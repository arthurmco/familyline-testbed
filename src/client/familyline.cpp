
/***
    Familyline main file

    Copyright 2016, 2017, 2019-2020 Arthur Mendes.

***/

#include <fmt/core.h>

#include <chrono>

#include "client/graphical/gui/gui_container_component.hpp"
#define GLM_FORCE_RADIANS

#ifndef _WIN32
#include <sys/utsname.h>
#include <unistd.h>
#endif

#ifdef _WIN32

#define _WINSOCKAPI_
#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ws2tcpip.h>
#define usleep(x) Sleep(x / 1000);

#endif

#include <fmt/format.h>

#include <atomic>
#include <cinttypes>
#include <client/HumanPlayer.hpp>
#include <client/config_reader.hpp>
#include <client/game.hpp>
#include <client/graphical/device.hpp>
#include <client/graphical/framebuffer.hpp>
#include <client/graphical/gui/gui_button.hpp>
#include <client/graphical/gui/gui_checkbox.hpp>
#include <client/graphical/gui/gui_imageview.hpp>
#include <client/graphical/gui/gui_label.hpp>
#include <client/graphical/gui/gui_listbox.hpp>
#include <client/graphical/gui/gui_textbox.hpp>
#include <client/graphical/gui/gui_manager.hpp>
#include <client/graphical/gui/gui_window.hpp>
#include <client/graphical/renderer.hpp>
#include <client/graphical/shader_manager.hpp>
#include <client/graphical/window.hpp>
#include <client/input/InputPicker.hpp>
#include <client/input/input_service.hpp>
#include <client/loop_runner.hpp>
#include <client/params.hpp>
#include <client/player_enumerator.hpp>
#include <common/logger.hpp>
#include <common/logic/input_recorder.hpp>
#include <common/logic/input_reproducer.hpp>
#include <common/net/server_finder.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>  //glm::lookAt()
#include <thread>

using namespace familyline;
using namespace familyline::logic;
using namespace familyline::graphics;
using namespace familyline::graphics::gui;
using namespace familyline::input;
using namespace familyline::net;

#ifdef WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#endif

#ifdef _MSC_VER
#undef main  // somehow vs does not find main()

#endif

/**
 * Initialize global sockets.
 *
 * Currently only meaningful on Windows because of that WSADATA thing
 *
 * Yes, this will be moved out of here.
 */
bool init_network()
{
#ifdef WIN32
    WSADATA trash;
    if (WSAStartup(MAKEWORD(2, 0), &trash) != 0) return false;
#endif

    return true;
}

/**
 * Enable console color on Windows 8 or newer
 *
 * On other operating systems, this is a noop
 */
void enable_console_color(FILE* f)
{
#ifdef _WIN32
    if (f == stdout || f == stderr) {
        // Set output mode to handle virtual terminal sequences, so the
        // color escape sequences can work on Windows
        HANDLE hOut = GetStdHandle(f == stderr ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) {
            GetLastError();
        }

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) {
            GetLastError();
        }

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(hOut, dwMode)) {
            GetLastError();
        }
    }
#endif
}

#ifdef _WIN32
std::string get_windows_version(const OSVERSIONINFOEX& osvi)
{
    if (osvi.dwMajorVersion < 6) {
        return fmt::format("version {}.{} (Unsupported)", osvi.dwMajorVersion, osvi.dwMinorVersion);
    }

    std::string version;
    if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 &&
        osvi.wProductType == VER_NT_WORKSTATION)
        version = "Windows Vista";
    else if (
        osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 &&
        osvi.wProductType != VER_NT_WORKSTATION)
        version = "Windows Server 2008";
    else if (
        osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 &&
        osvi.wProductType == VER_NT_WORKSTATION)
        version = "Windows 7";
    else if (
        osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 &&
        osvi.wProductType != VER_NT_WORKSTATION)
        version = "Windows Server 2008 R2";
    else if (
        osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 &&
        osvi.wProductType == VER_NT_WORKSTATION)
        version = "Windows 8";
    else if (
        osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 &&
        osvi.wProductType != VER_NT_WORKSTATION)
        version = "Windows Server 2012";
    else if (
        osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3 &&
        osvi.wProductType == VER_NT_WORKSTATION)
        version = "Windows 8.1";
    else if (
        osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3 &&
        osvi.wProductType != VER_NT_WORKSTATION)
        version = "Windows Server 2012 R2";
    else if (
        osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 &&
        osvi.wProductType == VER_NT_WORKSTATION)
        version = "Windows 10";
    else if (
        osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 &&
        osvi.wProductType != VER_NT_WORKSTATION)
        version = "Windows Server 2016";
    else
        version = fmt::format("Windows {}.{}", osvi.dwMajorVersion, osvi.dwMinorVersion);

    return fmt::format("{} {}", version, osvi.szCSDVersion);
}
#endif

/**
 * Get current system name
 *
 * The first string of the tuple returns the system name, in a more basic way
 * The second returns the version
 * The third returns more information:
 *    - On Linux, it returns the distro name and version
 *    - On Windows, it returns the kernel version (like 2004)
 *
 * This is only useful to diagnostics, and to do telemetry in the future
 */
std::tuple<std::string, std::string, std::string> get_system_name()
{
    std::string sysname = std::string{SDL_GetPlatform()};

#ifdef _WIN32

    /* Microsoft wants to get rid of GetVersionEx because other people
     * use them to determine features
     *
     * I only need to get the current version to show to people, so please, have mercy :)
     *
     * YES, I know it is deprecated.
     */
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    GetVersionEx((LPOSVERSIONINFO)&osvi);

    std::string version = get_windows_version(osvi);
    std::string sysinfo = fmt::format("Build {}", osvi.dwBuildNumber);
#else
    // all others probably are unix
    std::string version;
    std::string sysinfo;

    struct utsname uts;
    if (uname(&uts) == 0) {
        version = fmt::format("{} {}", uts.sysname, uts.release);
        sysinfo = fmt::format("{}", uts.version);
    }

#endif

    return std::tie(sysname, version, sysinfo);
}

static int show_starting_menu(
    const ParamInfo& pi, Framebuffer* f3D, Framebuffer* fGUI, graphics::Window* win,
    GUIManager* guir, size_t gwidth, size_t gheight, LoopRunner& lr, ConfigData& confdata);

/**
 * Information required to start a game, besides player information
 */
struct StartGameInfo {
    /// The terrain file
    std::string mapFile;

    /// If present, the input record file.
    /// If this is present, it means that we have a recorded game.
    std::optional<std::string> inputFile;
};

template <class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

Game* start_game(
    Framebuffer* f3D, Framebuffer* fGUI, graphics::Window* win, GUIManager* guir, LoopRunner& lr,
    const StartGameInfo& sgai, ConfigData& confdata)
{
    GFXGameInit gi{
        win,
        f3D,
        fGUI,
        guir,
    };

    auto& log = LoggerService::getLogger();

    std::string mapfile = sgai.mapFile;
    std::unique_ptr<InputReproducer> irepr;

    if (sgai.inputFile) {
        irepr = std::make_unique<InputReproducer>(*sgai.inputFile);

        if (!irepr->open()) {
            mapfile = ASSET_FILE_DIR "terrain_test.flte";
            irepr.reset();
            win->showMessageBox(
                "Familyline error", SysMessageBoxFlags::Error,
                fmt::format(
                    "Could not open input record {}\n"
                    "Check the logs for more information.",
                    *sgai.inputFile));
            return nullptr;

        } else {
            mapfile = irepr->getTerrainFile();
        }
    }

    Game* g   = new Game(gi);
    auto& map = g->initMap(mapfile);

    std::string player_name = "Arthur";
    auto pinfo              = InitPlayerInfo{player_name, -1};

    std::unique_ptr<PlayerManager> pm;
    std::unique_ptr<ColonyManager> cm;
    unsigned int human_player_id = 0;
    PlayerSession session;
    g->initAssets();
    ObjectFactory* of = g->initObjectFactory();

    auto createNormalSession_fn = [&]() {
        /// running a normal game
        session         = initSinglePlayerSession(map, pinfo);
        pm              = std::move(session.players);
        cm              = std::move(session.colonies);
        human_player_id = pinfo.id;
    };

    if (irepr) {
        if (!irepr->verifyObjectChecksums(of)) {
            irepr.reset();
            win->showMessageBox(
                "Familyline error", SysMessageBoxFlags::Error,
                fmt::format(
                    "Could not open input record {}, objects do not match\n"
                    "The person who recorded might have a mod you do not have, or a"
                    "different version than you.",
                    *sgai.inputFile));
            return nullptr;

        } else {
            /// loading input from a file
            session = irepr->createPlayerSession(map);
            pm      = std::move(session.players);
            cm      = std::move(session.colonies);

            human_player_id = pm->add(std::unique_ptr<Player>(
                new HumanPlayer{*pm.get(), map, player_name.c_str(), 0, false}));
            auto* player    = *(pm->get(human_player_id));
            auto& alliance  = cm->createAlliance(std::string{player->getName()});
            auto& colony    = cm->createColony(
                *player, 0xffffff, std::optional<std::reference_wrapper<Alliance>>{alliance});
            session.player_colony.emplace(human_player_id, std::reference_wrapper(colony));
        }
    } else {
        createNormalSession_fn();
    }

    if (human_player_id == -1) {
        throw std::runtime_error{"Could not create the human player"};
    }

    std::unique_ptr<InputRecorder> ir;

    if (confdata.enableInputRecording && !irepr) {
        log->write("game", LogType::Info, "This game inputs will be recorded");

        ir = std::make_unique<InputRecorder>(*pm.get());

        auto rawtime        = time(NULL);
        auto ftime          = localtime(&rawtime);
        auto recordfilename = fmt::format(
            "record-{}{:04d}{:02d}-{:02d}{:02d}{:02d}.frec", 1900 + ftime->tm_year, ftime->tm_mon,
            ftime->tm_mday, ftime->tm_hour, ftime->tm_min, ftime->tm_sec);
        std::filesystem::path recordpath(confdata.defaultInputRecordDir);
        recordpath.make_preferred() /= recordfilename;

        log->write("game", LogType::Info, "\trecord destination: %s", recordpath.c_str());

        if (!ir->createFile(recordpath.string(), of)) {
            log->write("game", LogType::Error, "\tinput record file could not be created");
            ir = std::unique_ptr<InputRecorder>(nullptr);
        }

        // You can't reproduce a record while recording.
        //
        // It will work, will not overwrite any file (because the filename is decided by the
        // recording date), but it is redundant.
        g->initRecorder(std::move(ir));
    }

    if (irepr) {
        log->write(
            "game", LogType::Info, "Replaying inputs from file %s", (*sgai.inputFile).c_str());

        irepr->reset();

        // dispatch 10 seconds worth of events
        irepr->dispatchEvents((1000 / LOGIC_DELTA) * 1);
        g->initReproducer(std::move(irepr));
    }

    g->initPlayers(std::move(pm), std::move(cm), session.player_colony, human_player_id);
    g->initObjectManager();
    g->initLoopData(human_player_id);

    return g;
}

void run_game_loop(LoopRunner& lr, int& framecount)
{
    while (true) {
        if (!lr.run()) {
            break;
        }

        framecount++;
    }
}

/////////
/////////
/////////

ConfigData read_settings()
{
    ConfigData confdata;

    if (auto confenv = getenv("CONFIG_FILE_PATH"); confenv) {
        fprintf(stderr, "reading config file from envvar (%s)\n", confenv);
        read_config_from(confenv, confdata);
        return confdata;
    }

    auto confpaths = get_config_valid_paths();
    for (auto& confpath : confpaths) {
        read_config_from(confpath, confdata);
    }

    return confdata;
}

int main(int argc, char const* argv[])
{
    auto pi = parse_params(std::vector<std::string>{argv + 1, argv + argc});
    enable_console_color(pi.log_device);

    fmt::print("Using resolution {} x {} \n", pi.width, pi.height);
    ConfigData confdata = read_settings();

    LoggerService::createLogger(pi.log_device, LogType::Info, confdata.log.blockTags);

    auto& log = LoggerService::getLogger();

    auto [sysname, sysversion, sysinfo] = get_system_name();

    log->write("", LogType::Info, "Familyline " VERSION);
    log->write("", LogType::Info, "built on " __DATE__ " by " USERNAME);
#if defined(COMMIT)
    log->write("", LogType::Info, "git commit is " COMMIT);
#endif
    log->write("", LogType::Info, "Running on OS %s", sysname.c_str());
    log->write("", LogType::Info, "  version: %s (%s)", sysversion.c_str(), sysinfo.c_str());

    log->write("", LogType::Info, "Using renderer '%s'", pi.renderer.c_str());

    char timestr[32];

    auto tm     = time(NULL);
    auto tminfo = localtime(&tm);
    strftime(timestr, 32, "%F %T", tminfo);
    log->write("", LogType::Info, "Actual date is %s", timestr);

    log->write("", LogType::Info, "Default model directory is " MODELS_DIR);
    log->write("", LogType::Info, "Default texture directory is " TEXTURES_DIR);
    log->write("", LogType::Info, "Default material directory is " MATERIALS_DIR);

    LoopRunner lr;

    graphics::Window* win = nullptr;
    GUIManager* guir      = nullptr;
    PlayerManager* pm     = nullptr;
    Game* g               = nullptr;

    try {
        auto devs          = pi.devices;
        Device* defaultdev = nullptr;

        if (devs.size() == 0) {
            log->write("main", LogType::Fatal, "no video devices found!");
            return 1;
        }

        for (auto d : devs) {
            log->write(
                "main", LogType::Info, "driver found: %s %s", d->getCode().data(),
                (d->isDefault() ? "(default)" : ""));

            if (d->isDefault()) defaultdev = d;
        }

        if (!defaultdev) {
            log->write(
                "main", LogType::Warning, "no default device available, choosing the first one");
            defaultdev = devs[0];
        }
        GFXService::setDevice(std::unique_ptr<Device>(defaultdev));

        InputProcessor* ipr = new InputProcessor;
        InputService::setInputManager(std::make_unique<InputManager>(*ipr));
        auto& ima = InputService::getInputManager();

        //        InputManager::GetInstance()->Initialize();
        win = GFXService::getDevice()->createWindow(pi.width, pi.height);

        win->show();
        // enable_gl_debug();

        if (!init_network()) {
            throw net_exception("Could not initialize network");
        }

        log->write("", LogType::Info, "Device name: %s", defaultdev->getName().data());
        log->write("", LogType::Info, "Device vendor: %s ", defaultdev->getVendor().data());

        int fwidth, fheight;
        int gwidth, gheight;
        win->getFramebufferSize(fwidth, fheight);
        win->getFramebufferSize(gwidth, gheight);

        Framebuffer* f3D  = GFXService::getDevice()->createFramebuffer("f3D", fwidth, fheight);
        Framebuffer* fGUI = GFXService::getDevice()->createFramebuffer("fGUI", gwidth, gheight);
        win->setFramebuffers(f3D, fGUI);

        guir = win->createGUIManager();
        guir->initialize(*win);
        // guir->initShaders(win);

        /* If we have a networked game ready, don't even show the main menu. */
        log->write(
            "texture", LogType::Info, "maximum tex size: %zu x %zu", Texture::GetMaximumSize(),
            Texture::GetMaximumSize());

        if (pi.mapFile || pi.inputFile) {
            int frames = 0;

            std::string mapFile = pi.inputFile ? "" : *pi.mapFile;

            Game* g = start_game(
                f3D, fGUI, win, guir, lr, StartGameInfo{mapFile, pi.inputFile}, confdata);

            if (g) {
                lr.load([&]() { return g->runLoop(); });
                run_game_loop(lr, frames);
            }

            if (g) delete g;

            delete win;
            delete f3D;
            delete fGUI;
            fmt::print("\nExited. ({:d} frames)\n", frames);

        } else {
            return show_starting_menu(pi, f3D, fGUI, win, guir, gwidth, gheight, lr, confdata);
        }

    } catch (shader_exception& se) {
        log->write("init", LogType::Fatal, "Shader error: %s [d]", se.what());

        if (win) {
            std::string content = fmt::format(
                "Shader {} could not be compiled\n"
                "\n"
                "Error: {}",
                se.file, se.what());
            win->showMessageBox("Familyline Error", SysMessageBoxFlags::Error, content);
        }

        exit(EXIT_FAILURE);
    } catch (graphical_exception& we) {
        log->write("init", LogType::Fatal, "Window creation error: %s (d)", we.what());

        if (win) {
            std::string content = fmt::format("Graphical error: {}\n", we.what());
            win->showMessageBox("Familyline Error", SysMessageBoxFlags::Error, content);
        }

        exit(EXIT_FAILURE);
    } catch (logic_exception& le) {
        log->write("init", LogType::Fatal, "Logic exception: %s (d)", le.what());

        if (win) {
            std::string content = fmt::format(
                "An error happened in the simulation part of the game:\n"
                "\n"
                "Error: {}",
                le.what());
            win->showMessageBox("Familyline Error", SysMessageBoxFlags::Error, content);
        }

        exit(EXIT_FAILURE);
    } catch (net_exception& le) {
        log->write("init", LogType::Fatal, "Network exception: %s (d)", le.what());

        if (win) {
            std::string content = fmt::format(
                "A network-related fatal error happened:\n"
                "\n"
                "Error: {}",
                le.what());
            win->showMessageBox("Familyline Error", SysMessageBoxFlags::Error, content);
        }

        exit(EXIT_FAILURE);
    } catch (std::bad_alloc& be) {
        log->write("init", LogType::Fatal, "Allocation error: %s", be.what());

        log->write("init", LogType::Fatal, "Probably out of memory");

        if (win) {
            std::string content = fmt::format("Insufficient memory\n\nError: {:s}", be.what());
            win->showMessageBox("Familyline Error", SysMessageBoxFlags::Error, content);
        }

        exit(EXIT_FAILURE);
    }
}

static int show_starting_menu(
    const ParamInfo& pi, Framebuffer* f3D, Framebuffer* fGUI, graphics::Window* win,
    GUIManager* guir, size_t gwidth, size_t gheight, LoopRunner& lr, ConfigData& confdata)
{
    std::thread async_test;
    std::atomic<int> val    = 0;
    std::atomic<bool> aexit = false;

    auto sf = ServerFinder();

    auto& log = LoggerService::getLogger();
    Game* g   = nullptr;
    auto& ima = InputService::getInputManager();
    /* Render the menu */
    bool r = true;
    // auto deflistener = InputManager::GetInstance()->GetDefaultListener();

    GUIWindow* gwin = guir->createGUIWindow("main", gwidth, gheight);

    Label* l = new Label(0.37, 0.03, "FAMILYLINE");
    l->modifyAppearance([](ControlAppearance& ca) {
        ca.fontSize   = 32;
        ca.foreground = {1, 1, 1, 1};
    });

    Label* lv = new Label(0.32, 0.8, "Version " VERSION ", commit " COMMIT);
    lv->modifyAppearance([](ControlAppearance& ca) {
        ca.foreground = {0.2, 0.2, 1, 1};
        ca.background = {1, 1, 1, 0.5};
    });

    Button* bnew      = new Button(400, 50, "New Game");  // Button(0.1, 0.2, 0.8, 0.1, "New Game");
    Button* bsettings = new Button(400, 50, "Settings");  // Button(0.1, 0.2, 0.8, 0.1, "New Game");
    Button* bmplayer =
        new Button(400, 50, "Multiplayer");            // Button(0.1, 0.2, 0.8, 0.1, "New Game");
    Button* bquit = new Button(400, 50, "Exit Game");  // Button(0.1, 0.31, 0.8, 0.1, "Exit Game");

    ImageView* ilogo = new ImageView(300, 450);  // 0.2, 0.1, 0.6, 0.9,
    ilogo->loadFromFile(ICONS_DIR "/familyline-logo.png");

    ilogo->z_index = -100;
    // ilogo.SetZIndex(0.9);
    // ilogo.SetOpacity(0.5);

    bquit->setClickCallback([&r](Control* cc) {
        (void)cc;
        r = false;
    });

    bsettings->setClickCallback([&](auto* cc) {
        GUIWindow* gsettings = guir->createGUIWindow("settings", gwidth, gheight);
        // TODO: copy label?
        auto lb = std::make_unique<Label>(0.37, 0.03, "FAMILYLINE");
        lb->modifyAppearance([](ControlAppearance& ca) {
            ca.fontSize   = 32;
            ca.foreground = {1, 1, 1, 1};
        });

        auto header = std::make_unique<Label>(0.37, 0.03, "Settings");
        header->modifyAppearance([](ControlAppearance& ca) {
            ca.fontSize   = 24;
            ca.foreground = {1, 1, 1, 0.9};
        });

        auto txtname = std::make_unique<Textbox>(600, 40, "Nome do player");
        
        auto bret =
            std::make_unique<Button>(200, 50, "Return");  // Button(0.1, 0.2, 0.8, 0.1, "New Game");

        bret->setClickCallback([&](auto* c) {
            GUIWindow* gsettings          = guir->getGUIWindow("settings");
            Checkbox* recordGame          = (Checkbox*)gsettings->get("recordGame");
            confdata.enableInputRecording = recordGame->getState();
            guir->closeWindow(*gsettings);
            guir->destroyGUIWindow("settings");
        });

        gsettings->add(0.37, 0.03, ControlPositioning::CenterX, std::move(lb));
        gsettings->add(0.37, 0.13, ControlPositioning::CenterX, std::move(header));
        gsettings->add(
            0.05, 0.3, ControlPositioning::Relative,
            std::make_unique<Checkbox>(
                300, 32, "Record the game inputs", confdata.enableInputRecording),
            "recordGame");
        gsettings->add(0.05, 0.4, ControlPositioning::Relative, std::move(txtname));
        gsettings->add(0.37, 0.9, ControlPositioning::CenterX, std::move(bret));

        guir->showWindow(gsettings);
    });

    bmplayer->setClickCallback([&](auto* cc) {
        GUIWindow* gmplayer = guir->createGUIWindow("mplayer", gwidth, gheight);
        // TODO: copy label?
        auto lb = std::make_unique<Label>(0.37, 0.03, "FAMILYLINE");
        lb->modifyAppearance([](ControlAppearance& ca) {
            ca.fontSize   = 32;
            ca.foreground = {1, 1, 1, 1};
        });

        auto header = std::make_unique<Label>(0.37, 0.03, "Multiplayer");
        header->modifyAppearance([](ControlAppearance& ca) {
            ca.fontSize   = 24;
            ca.foreground = {1, 1, 1, 0.9};
        });

        auto info = std::make_unique<Label>(
            0.37, 0.03, "Select a server from the list below, and click in 'Connect'");
        info->modifyAppearance([](ControlAppearance& ca) { ca.foreground = {1, 1, 1, 1}; });

        auto disclbl = std::make_unique<Label>(
            0.37, 0.03, "<i>Discovering servers in your local network...</i>");
        disclbl->modifyAppearance([](ControlAppearance& ca) { ca.foreground = {1, 1, 1, 1}; });

        auto serverlist = std::make_unique<Listbox>(800 * 0.7, 600 * 0.4);

        aexit = false;
        val   = 0;

        auto bconnect = std::make_unique<Button>(
            200, 50, "Connect");  // Button(0.1, 0.2, 0.8, 0.1, "New Game");
        bconnect->modifyAppearance([](ControlAppearance& ca) {
            ca.foreground = {0, 0, 0, 0.9};
            ca.background = {1, 1, 1, 0.9};
        });

        auto bret =
            std::make_unique<Button>(200, 30, "Return");  // Button(0.1, 0.2, 0.8, 0.1, "New Game");

        bret->setClickCallback([&](auto* c) {
            GUIWindow* gmplayer = guir->getGUIWindow("mplayer");
            Listbox* slist      = (Listbox*)gmplayer->get("serverlist");

            aexit = true;
            printf("%s ==== \n", slist->getSelectedItem().c_str());

            sf.stopDiscover();
            guir->closeWindow(*gmplayer);
            guir->destroyGUIWindow("mplayer");
        });

        gmplayer->add(0.37, 0.03, ControlPositioning::CenterX, std::move(lb));
        gmplayer->add(0.37, 0.13, ControlPositioning::CenterX, std::move(header));
        gmplayer->add(0.05, 0.2, ControlPositioning::Relative, std::move(info));
        gmplayer->add(
            0.05, 0.25, ControlPositioning::Relative, std::move(serverlist), "serverlist");
        gmplayer->add(0.6, 0.775, ControlPositioning::Relative, std::move(bconnect));
        gmplayer->add(0.05, 0.8, ControlPositioning::Relative, std::move(disclbl));
        gmplayer->add(0.37, 0.9, ControlPositioning::CenterX, std::move(bret));

        sf.startDiscover([&](ServerInfo si) {
            fmt::print(
                " \033[1m{}\033[0m ({}/{})\n{}:{}\n\n", si.name, si.player_count, si.player_max,
                si.ip_addr, si.port);

            GUIWindow* gmplayer = guir->getGUIWindow("mplayer");
            Listbox* slist      = (Listbox*)gmplayer->get("serverlist");

            slist->addItem(
                si.ip_addr, std::make_unique<Label>(
                                0, 0,
                                fmt::format(
                                    "<b>{}</b>     - {}:{} <span foreground='cyan'>({}/{})</span>",
                                    si.name, si.ip_addr, si.port, si.player_count, si.player_max)));
        });

        guir->showWindow(gmplayer);
    });

    bnew->setClickCallback([&](Control* cc) {
        (void)cc;
        if (async_test.joinable()) {
            if (!aexit) {
                aexit = true;
                async_test.join();
            }
        }

        guir->closeWindow(*gwin);

        g = start_game(
            f3D, fGUI, win, guir, lr,
            StartGameInfo{ASSET_FILE_DIR "terrain_test.flte", std::nullopt}, confdata);

        if (g) {
            lr.load([&]() { return g->runLoop(); });
        }
    });

    gwin->add(0.37, 0.03, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)l));
    gwin->add(0.32, 0.8, ControlPositioning::Relative, std::unique_ptr<Control>((Control*)lv));
    gwin->add(0.1, 0.2, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)bnew));
    gwin->add(
        0.1, 0.305, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)bsettings));
    gwin->add(0.1, 0.41, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)bmplayer));
    gwin->add(0.1, 0.52, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)bquit));
    gwin->add(0.2, 0.01, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)ilogo));

    guir->showWindow(gwin);
    // guir->add(0, 0, ControlPositioning::Pixel, std::unique_ptr<Control>((Control*)gwin));

    ima->addListenerHandler([&](HumanInputAction hia) {
        /* Only listen for game exit events, because you sure want to
           close the window The others will be handled by the GUI listener */
        if (std::holds_alternative<GameExit>(hia.type)) {
            r = false;
        }

        return false;
    });

    double b   = SDL_GetTicks();
    int frames = 0;

    lr.load([&]() {
        // Input
        ima->processEvents();

        guir->receiveEvent();
        guir->runCallbacks();
        guir->update();

        // Render
        fGUI->startDraw();
        guir->render(0, 0);
        // guir->renderToScreen();
        fGUI->endDraw();

        win->update();

        double e = SDL_GetTicks();
        if ((e - b) < 1000 / 60.0) SDL_Delay((unsigned int)(1000 / 60.0 - (e - b)));
        b = SDL_GetTicks();
        return r;
    });

    run_game_loop(lr, frames);

    if (async_test.joinable()) {
        if (!aexit) {
            aexit = true;
            async_test.join();
        }
    }

    if (g) delete g;

    delete win;
    delete f3D;
    delete fGUI;
    fmt::print("\nExited. ({:d} frames)\n", frames);

    return 0;
}

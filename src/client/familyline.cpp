
/***
    Familyline main file

    Copyright 2016, 2017, 2019-2020 Arthur Mendes.

***/

#include <fmt/core.h>
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
#define usleep(x) Sleep(x / 1000);

#endif

#include <fmt/format.h>

#include <client/game.hpp>
#include <client/config_reader.hpp>
#include <client/player_enumerator.hpp>
#include <client/graphical/device.hpp>
#include <client/graphical/framebuffer.hpp>
#include <client/graphical/gui/gui_button.hpp>
#include <client/graphical/gui/gui_imageview.hpp>
#include <client/graphical/gui/gui_label.hpp>
#include <client/graphical/gui/gui_checkbox.hpp>
#include <client/graphical/gui/gui_manager.hpp>
#include <client/graphical/gui/gui_window.hpp>
#include <client/graphical/renderer.hpp>
#include <client/graphical/shader_manager.hpp>
#include <client/graphical/window.hpp>
#include <client/input/InputPicker.hpp>
#include <client/input/input_service.hpp>
#include <client/loop_runner.hpp>
#include <client/params.hpp>
#include <common/logger.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>  //glm::lookAt()

using namespace familyline;
using namespace familyline::logic;
using namespace familyline::graphics;
using namespace familyline::graphics::gui;
using namespace familyline::input;

#ifdef WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#endif

#ifdef _MSC_VER
#undef main  // somehow vs does not find main()

#endif

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
            return GetLastError();
        }

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) {
            return GetLastError();
        }

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(hOut, dwMode)) {
            return GetLastError();
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
    GUIManager* guir, size_t gwidth, size_t gheight, LoopRunner& lr);

Game* start_game(
    Framebuffer* f3D, Framebuffer* fGUI, graphics::Window* win, GUIManager* guir, LoopRunner& lr,
    std::string_view mapFile)
{
    GFXGameInit gi{
        win,
        f3D,
        fGUI,
        guir,
    };


    Game* g = new Game(gi);
    auto& map = g->initMap(mapFile);
    auto pinfo = InitPlayerInfo{"Arthur", -1};
    auto session = initSinglePlayerSession(map, pinfo);

    auto pm = std::move(session.players);
    auto cm = std::move(session.colonies);
    
    if (pinfo.id == -1) {
        throw std::runtime_error{"Could not create the human player"};
    }

    g->initPlayers(std::move(pm), std::move(cm), session.player_colony, pinfo.id);
    g->initObjects();
    g->initLoopData(pinfo.id);

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

        if (pi.mapFile) {
            int frames = 0;
            Game* g    = start_game(f3D, fGUI, win, guir, lr, *pi.mapFile);
            lr.load([&]() { return g->runLoop(); });

            run_game_loop(lr, frames);

            if (g) delete g;

            delete win;
            delete f3D;
            delete fGUI;
            fmt::print("\nExited. ({:d} frames)\n", frames);

        } else {
            return show_starting_menu(pi, f3D, fGUI, win, guir, gwidth, gheight, lr);
        }

    } catch (shader_exception& se) {
        log->write("init", LogType::Fatal, "Shader error: %s [d]", se.what());

        if (win) {
            //            fmt::memory_buffer out;
            //            format_to(out,
            //                "Familyline found an error in a shader\n"
            //                "\n"
            //                "Error: {:s}\n"
            //                "File: {:s}, type: {:d}, code: {:d}",
            //                se.what(), se.file.c_str(), se.type, se.code);
            //            w->showMessageBox(out.data(), "Error", MessageBoxInfo::Error);
        }

        exit(EXIT_FAILURE);
    } catch (graphical_exception& we) {
        log->write("init", LogType::Fatal, "Window creation error: %s (d)", we.what());

        fmt::print(stderr, "Error while creating the window: {:s}\n", we.what());

        exit(EXIT_FAILURE);
    } catch (std::bad_alloc& be) {
        log->write("init", LogType::Fatal, "Allocation error: %s", be.what());

        log->write("init", LogType::Fatal, "Probably out of memory");

        if (win) {
            //            fmt::memory_buffer out;
            //            format_to(out,
            //                "Insufficient memory\n"
            //                "\n"
            //                "Error: {:s}",
            //                be.what());
            //            w->ShowMessageBox(out.data(), "Error", MessageBoxInfo::Error);
        }

        exit(EXIT_FAILURE);
    }
}

static int show_starting_menu(
    const ParamInfo& pi, Framebuffer* f3D, Framebuffer* fGUI, graphics::Window* win,
    GUIManager* guir, size_t gwidth, size_t gheight, LoopRunner& lr)
{
    auto& log = LoggerService::getLogger();
    Game* g   = nullptr;
    auto& ima = InputService::getInputManager();
    /* Render the menu */
    bool r = true;
    // auto deflistener = InputManager::GetInstance()->GetDefaultListener();

    GUIWindow* gwin      = new GUIWindow(gwidth, gheight);
    GUIWindow* gsettings = new GUIWindow(gwidth, gheight);
    // TODO: copy label?
    Label* lb = new Label(0.37, 0.03, "FAMILYLINE");
    lb->modifyAppearance([](ControlAppearance& ca) {
        ca.fontSize   = 32;
        ca.foreground = {1, 1, 1, 1};
    });

    Label* header = new Label(0.37, 0.03, "Settings");
    header->modifyAppearance([](ControlAppearance& ca) {
        ca.fontSize   = 24;
        ca.foreground = {1, 1, 1, 0.9};
    });

    Button* bret = new Button(200, 50, "Return");  // Button(0.1, 0.2, 0.8, 0.1, "New Game");
    bret->setClickCallback([&](auto* c) { guir->closeWindow(*gsettings); });
    
    Checkbox* recordGame = new Checkbox(300, 32, "Record the game inputs");
    
    gsettings->add(0.37, 0.03, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)lb));
    gsettings->add(
        0.37, 0.13, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)header));
    gsettings->add(
        0.05, 0.3, ControlPositioning::Relative, std::unique_ptr<Control>((Control*)recordGame));
    gsettings->add(
        0.37, 0.9, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)bret));

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

    bsettings->setClickCallback([&](auto* cc) { guir->showWindow(gsettings); });

    bnew->setClickCallback([&](Control* cc) {
        (void)cc;
        guir->closeWindow(*gwin);
        g = start_game(f3D, fGUI, win, guir, lr, ASSET_FILE_DIR "terrain_test.flte");
        lr.load([&]() { return g->runLoop(); });
    });

    gwin->add(0.37, 0.03, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)l));
    gwin->add(0.32, 0.8, ControlPositioning::Relative, std::unique_ptr<Control>((Control*)lv));
    gwin->add(0.1, 0.2, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)bnew));
    gwin->add(
        0.1, 0.305, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)bsettings));
    gwin->add(0.1, 0.410, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)bquit));
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

    if (g) delete g;

    delete gsettings;
    delete gwin;

    delete win;
    delete f3D;
    delete fGUI;
    fmt::print("\nExited. ({:d} frames)\n", frames);

    return 0;
}

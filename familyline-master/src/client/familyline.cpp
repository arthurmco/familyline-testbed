
/***
    Familyline main file

    Copyright 2016, 2017, 2019-2020 Arthur Mendes.

***/
#include <fmt/core.h>
#include <fmt/format.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <client/familyline.hpp>
#include <common/logic/script_environment.hpp>
#include <common/net/net_player_sender.hpp>
#include <common/net/network_player.hpp>
#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>  //glm::lookAt()
#include <iterator>
#include <memory>
#include <thread>

using namespace familyline;
using namespace familyline::logic;
using namespace familyline::graphics;
using namespace familyline::graphics::gui;
using namespace familyline::input;
using namespace familyline::net;

// TODO: create a shader manager *and* a texture manager

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

void end_network()
{
#ifdef WIN32
    WSACleanup();
#endif
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

/**
 * This function creates a game entity, that will represents the state of a game
 *
 * We pass some interface information, a loop runner, parameter data (the StartGameinfo),
 * configuration data, and a function that returns a session, i.e, the objects for players
 * and teams (we call them colonies)
 *
 * Since multiplayer and singleplayer have different player objects, we need this callback
 * to create them
 */
template <typename SessionF>
Game* start_game(
    GraphicalInfo ginfo, LoopRunner& lr, const StartGameInfo& sgai, ConfigData& confdata,
    SessionF fnSession);

/**
 * Wait for a list futures promises, then return
 *
 * Return true when every item on the promise resolves
 *
 * The FnValidator is a function that you should call, and the cancel is a variable
 * you should set when you want to cancel the operation
 */
template <typename Result, typename Expect, std::invocable<Expect&> FnValidator>
bool waitFutures(
    std::vector<std::pair<Result, std::future<Expect>>>& promises, std::vector<Result>& result,
    FnValidator&& validator, auto& cancel)
{
    bool all_loaded = false;
    while (!all_loaded) {
        if (cancel) break;

        for (auto& [id, promise] : promises) {
            if (!promise.valid()) continue;

            //            printf("waiting on %zu\n", id);

            if (auto fstatus = promise.wait_for(std::chrono::milliseconds(100));
                fstatus == std::future_status::ready) {
                printf("%zu is ready\n", id);

                auto res = promise.get();
                if (!res.has_value()) {
                    printf("\tan error occurred while handling message for %lu\n", id);
                    continue;
                }

                if (validator(res)) {
                    result.push_back(id);
                }
            }

            if (result.size() == promises.size()) {
                all_loaded = true;
            }
        }
    }

    return all_loaded;
}

int start_networked_game(
    GraphicalInfo& ginfo, GamePacketServer& gps, std::vector<NetworkClient>& clients,
    ConfigData& cdata)
{
    printf("Loading the game...\n");
    printf("\tWaiting for the other clients to load\n");
    assert(clients.size() > 0);

    bool all_loaded            = false;
    bool all_ready             = false;
    std::atomic<bool> quitting = false;

    std::atomic<int> watchdog = 0;

    std::thread netthread([&]() {
        watchdog = 0;
        while (!quitting) {
            watchdog++;

            gps.update();
            std::for_each(clients.begin(), clients.end(), [&](NetworkClient& c) { c.update(); });

            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            // 1 min timeout
            if (watchdog == 1000 * 30) {
                puts("TIMED OUT");
                break;
            }
        }
    });

    std::vector<std::pair<uint64_t, std::future<tl::expected<bool, NetResult>>>> loading_promises;
    std::transform(
        clients.begin(), clients.end(), std::back_inserter(loading_promises),
        [](NetworkClient& c) { return std::make_pair(c.id(), c.waitLoading()); });
    std::vector<uint64_t> loading_clients;

    gps.sendLoadingMessage(0);
    watchdog = 0;
    /// TODO: load the map, assets, game class...

    /// TODO: fix an issue where two messages can come on the same TCP packet.
    /// This might happen here.
    /// Fix here and on Rust server, but only when receiving.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    gps.sendLoadingMessage(100);
    watchdog = 0;
    printf("\t our game loaded! \n");

    all_loaded = waitFutures(
        loading_promises, loading_clients, [&](auto& res) { return res.value_or(false) == true; },
        quitting);

    if (!all_loaded) {
        quitting = true;
        printf("\tanother client failed to load, exiting\n");
        netthread.join();
        return 1;
    }

    printf("\tnotifying other clients we are about to start, and waiting them\n");
    std::vector<std::pair<uint64_t, std::future<tl::expected<bool, NetResult>>>> start_promises;
    std::transform(
        clients.begin(), clients.end(), std::back_inserter(start_promises),
        [](NetworkClient& c) { return std::make_pair(c.id(), c.waitReadyToStart()); });
    std::vector<uint64_t> start_clients;

    std::this_thread::sleep_for(std::chrono::milliseconds(800));

    watchdog = 0;
    gps.sendStartMessage();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    LoopRunner lr;

    watchdog   = 0;
    int frames = 0;

    auto createMultiplayerSession_fn = [&](logic::Terrain& map, auto& local_player_info) {
        std::map<uint64_t /*player_id*/, std::reference_wrapper<logic::Colony>> player_colony;

        auto pm = std::make_unique<PlayerManager>();
        for (auto& c : clients) {
            pm->add(std::make_unique<NetworkPlayer>(*pm.get(), map, c), false);
        }
        auto hid = pm->add(
            std::unique_ptr<Player>(new HumanPlayer{
                *pm.get(), map, local_player_info.name.c_str(), gps.id(),
                *input::InputService::getCommandTable().get(), true}),
            false);
        local_player_info.id = hid;
        printf("\n%lx %lx\n", hid, gps.id());

        auto cm = std::make_unique<ColonyManager>();
        for (auto& c : clients) {
            auto* player = *pm->get(c.id());
            int color    = (int)player->getCode() & 0x00ffffff;

            auto& alliance = cm->createAlliance("alliance");
            auto& colony   = cm->createColony(
                  *player, color, std::optional<std::reference_wrapper<Alliance>>{alliance});

            player_colony.emplace(c.id(), std::reference_wrapper(colony));
        }

        {
            auto* player = *pm->get(hid);
            int color    = (int)player->getCode() & 0x00ffffff;

            auto& alliance = cm->createAlliance("alliance");
            auto& colony   = cm->createColony(
                  *player, color, std::optional<std::reference_wrapper<Alliance>>{alliance});

            player_colony.emplace(hid, std::reference_wrapper(colony));
        }

        return PlayerSession{std::move(pm), std::move(cm), player_colony};
    };

    watchdog = 0;
    Game* g  = start_game(
         ginfo, lr, StartGameInfo{ASSET_FILE_DIR "terrain_test.flte", std::nullopt}, cdata,
         createMultiplayerSession_fn);

    NetPlayerSender nps{*g->getPlayerManager(), gps, g->getHumanPlayerID()};

    all_ready = waitFutures(
        start_promises, start_clients, [&](auto& res) { return res.value_or(false) == true; },
        quitting);

    if (!all_ready) {
        quitting = true;
        printf("\tanother client failed to start the game, exiting\n");
        netthread.join();
        return 1;
    }

    printf("\tstarting the game\n");
    watchdog = 0;

    lr.load([&]() {
        watchdog = 0;
        return g->runLoop();
    });
    run_game_loop(lr, frames);

    quitting = true;
    netthread.join();

    return 0;
}

void start_networked_game_room(
    GraphicalInfo& ginfo, CServer& cserv, std::function<void(std::string, NetResult)> errHandler,
    ConfigData& cdata)
{
    CServerInfo si = {};
    auto ret       = cserv.getServerInfo(si);
    if (ret != NetResult::OK) {
        errHandler("Failure to get server info", ret);
        return;
    }

    std::optional<GamePacketServer> gps;

    bool exit = false;
    while (!exit) {
        fmt::print("\033[2J\n\033[1;1H");
        fmt::print("\033[1m{}\033[0m\t ({})\n", si.name, cserv.getAddress());
        fmt::print("\t\033[32m{}\033[0m/{} clients\n", si.clients.size(), si.max_clients);
        fmt::print("\n---------------------------------------------------\n");

        for (auto& cli : si.clients) {
            std::string readycolor = cli.ready ? "\033[34m" : "";
            fmt::print(
                " {} {}{}\033[0m\n", cli.id == cserv.getUserID() ? "=>" : "  ", readycolor,
                cli.name);
        }

        fmt::print("\033[{}B", si.max_clients - si.clients.size() + 1);
        fmt::print("\033[3B [(u)pdate, (q)uit, (c)onnect, (t)oggle ready]> ");

        fflush(stdin);
        auto v = getchar();

        switch (v) {
            case 'q':
            case 'Q': exit = true; break;
            case 'u':
            case 'U':
                ret = cserv.getServerInfo(si);
                if (ret != NetResult::OK) {
                    errHandler("Failure to get server info", ret);
                    return;
                }
                break;
            case 't':
            case 'T':
                ret = cserv.toggleReady(!cserv.isReady());
                if (ret != NetResult::OK) {
                    errHandler("Failure to set ready", ret);
                    return;
                }

                ret = cserv.getServerInfo(si);
                if (ret != NetResult::OK) {
                    errHandler("Failure to get server info", ret);
                    return;
                }

                if (std::all_of(
                        si.clients.begin(), si.clients.end(),
                        [](auto& v) { return v.ready == true; }) &&
                    si.clients.size() >= 2) {
                    cserv.connect();
                }

                break;
            case 'c':
            case 'C':
                ret = cserv.getServerInfo(si);
                if (ret != NetResult::OK) {
                    errHandler("Failure to get server info", ret);
                    return;
                }

                ret = cserv.connect();
                if (ret != NetResult::OK) {
                    errHandler("Failure to get the game server info", ret);

                } else {
                    printf("connecting to the game server\n");
                    gps = cserv.getGameServer();
                    if (!gps) {
                        printf("error: connection failed");
                        continue;
                    }

                    if (!gps->connect()) {
                        printf("error: failed to connect to server");
                        continue;
                    }

                    std::vector<NetworkClient> clients;
                    auto clientfut = gps->waitForClientConnection();
                    int cycles     = 0;
                    for (;;) {
                        gps->update();

                        auto fstatus = clientfut.wait_for(std::chrono::milliseconds(1));
                        if (fstatus == std::future_status::ready) {
                            clients = clientfut.get()
                                          .map_error([](auto e) {
                                              switch (e) {
                                                  case NetResult::ConnectionTimeout:
                                                      printf(
                                                          "Timed out while waiting other players. "
                                                          "Please try again.\n");
                                                      break;

                                                  case NetResult::UnexpectedDisconnect:
                                                      printf(
                                                          "The client disconnected, but it should "
                                                          "not disconnect!");
                                                      break;

                                                  case NetResult::ServerError:
                                                      printf("Unknown server error.\n");
                                                      break;

                                                  default: printf("Unexpected error\n");
                                              }

                                              return e;
                                          })
                                          .value_or(std::vector<NetworkClient>{});
                            break;
                        }
                    }

                    if (clients.size() <= 0) {
                        printf("No clients.\n");
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                        continue;
                    }

                    start_networked_game(ginfo, *gps, clients, cdata);
                    exit = true;
                }

                fflush(stdout);
                sleep(3);
                break;
        }
    }
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
    const ParamInfo& pi, GraphicalInfo ginfo, LoopRunner& lr, ConfigData& confdata);

template <class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

template <typename SessionF>
Game* start_game(
    GraphicalInfo ginfo, LoopRunner& lr, const StartGameInfo& sgai, ConfigData& confdata,
    SessionF fnSession)
{
    GFXGameInit gi{
        ginfo.win,
        ginfo.f3D,
        ginfo.fGUI,
        ginfo.guir,
    };

    auto& log = LoggerService::getLogger();

    std::string mapfile = sgai.mapFile;
    std::unique_ptr<InputReproducer> irepr;

    if (sgai.inputFile) {
        irepr = std::make_unique<InputReproducer>(*sgai.inputFile);

        if (!irepr->open()) {
            mapfile = ASSET_FILE_DIR "terrain_test.flte";
            irepr.reset();
            ginfo.win->showMessageBox(
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

    std::string player_name = confdata.player.username;
    auto pinfo              = InitPlayerInfo{player_name, uint64_t(-1)};

    std::unique_ptr<PlayerManager> pm;
    std::unique_ptr<ColonyManager> cm;
    uint64_t human_player_id = 0;
    PlayerSession session;
    g->initAssets();
    ObjectFactory* of = g->initObjectFactory();

    if (irepr) {
        if (!irepr->verifyObjectChecksums(of)) {
            irepr.reset();
            ginfo.win->showMessageBox(
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

            human_player_id = pm->add(std::unique_ptr<Player>(new HumanPlayer{
                *pm.get(), map, player_name.c_str(), 0,
                *input::InputService::getCommandTable().get(), false}));
            auto* player    = *(pm->get(human_player_id));
            auto& alliance  = cm->createAlliance(std::string{player->getName()});
            auto& colony    = cm->createColony(
                   *player, 0xffffff, std::optional<std::reference_wrapper<Alliance>>{alliance});
            session.player_colony.emplace(human_player_id, std::reference_wrapper(colony));
        }
    } else {
        session         = fnSession(map, pinfo);
        pm              = std::move(session.players);
        cm              = std::move(session.colonies);
        human_player_id = pinfo.id;
    }

    if (human_player_id == -1) {
        throw std::runtime_error{"Could not create the human player"};
    }

    // You can't reproduce a record while recording.
    //
    // It will work, will not overwrite any file (because the filename is decided by the
    // recording date), but it is redundant.
    if (confdata.enableInputRecording && !irepr) {
        log->write("game", LogType::Info, "This game inputs will be recorded");

        auto ir = std::make_unique<InputRecorder>(*pm.get());

        auto rawtime        = time(NULL);
        auto ftime          = localtime(&rawtime);
        auto recordfilename = fmt::format(
            "record-{}{:04d}{:02d}-{:02d}{:02d}{:02d}.frec", 1900 + ftime->tm_year, ftime->tm_mon,
            ftime->tm_mday, ftime->tm_hour, ftime->tm_min, ftime->tm_sec);
        std::filesystem::path recordpath(confdata.defaultInputRecordDir);
        recordpath.make_preferred() /= recordfilename;

        log->write("game", LogType::Info, "\trecord destination: {}", recordpath.string());

        if (!ir->createFile(recordpath.string(), of)) {
            log->write("game", LogType::Error, "\tinput record file could not be created");
        } else {
            g->initRecorder(std::move(ir));
        }
    }

    if (irepr) {
        log->write("game", LogType::Info, "Replaying inputs from file {}", sgai.inputFile);

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

void start_networked_game_cmdline(GraphicalInfo ginfo, std::string addr, ConfigData& cdata)
{
    auto treat_errors = [&](std::string msg, NetResult ret) {
        switch (ret) {
            case NetResult::ConnectionError:
                fmt::print("{}: {}\n", msg, fmt::format("Could not connect to address {}", addr));
                break;
            case NetResult::WrongPassword:
                fmt::print("{}: {}", msg, fmt::format("Server {} has a password", addr));
                break;
            case NetResult::LoginFailure:
                fmt::print(
                    "{}: {}\n", msg,
                    fmt::format(
                        "Could not log in to {}\nThe server was found, but we could not "
                        "login there.",
                        addr));
                break;
            case NetResult::ConnectionTimeout:
                fmt::print(
                    "{}: {}\n", msg,
                    fmt::format(
                        "Timed out while connecting to {}\nThe server probably does not "
                        "exist, or there is a firewall issue.",
                        addr));
                break;
            case NetResult::ServerError:
                fmt::print(
                    "{}: {}\n", msg,
                    fmt::format(
                        "Error while connecting to {}. The server sent incorrect data to "
                        "the client.",
                        addr));
                break;
            case NetResult::NotAllClientsConnected:
                fmt::print(
                    "{}: {}\n", msg,
                    fmt::format(
                        "Error where connecting to {}: The server reported that not all clients "
                        "were connected",
                        addr));
                break;
            default:
                fmt::print(
                    "{}: {}\n", msg,
                    fmt::format("Could not connect to address {}: unknown error {}", addr, ret));
        }
    };

    CServer cserv{};
    auto ret = cserv.login(addr, cdata.player.username);
    if (ret != NetResult::OK) {
        treat_errors("Connection error", ret);
        return;
    }

    start_networked_game_room(ginfo, cserv, treat_errors, cdata);

    cserv.logout();
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
    ParamInfo pi = parse_params(std::vector<std::string>{argv + 1, argv + argc});

    enable_console_color(pi.log_device);

    fmt::print("Using resolution {} x {} \n", pi.width, pi.height);
    ConfigData confdata = read_settings();

    LoggerService::createLogger(pi.log_device, LogType::Debug, confdata.log.blockTags);

    auto& log = LoggerService::getLogger();

    auto [sysname, sysversion, sysinfo] = get_system_name();

    log->write("", LogType::Info, "Familyline " VERSION);
    log->write("", LogType::Info, "built on " __DATE__ " by " USERNAME);
#if defined(COMMIT)
    log->write("", LogType::Info, "git commit is " COMMIT);
#endif
    log->write("", LogType::Info, "Running on OS {}", sysname);
    log->write("", LogType::Info, "  version: {} ({})", sysversion, sysinfo);

    log->write("", LogType::Info, "Using renderer '{}'", pi.renderer);

    auto tm     = time(NULL);
    auto tminfo = localtime(&tm);
    log->write("", LogType::Info, "Actual date is {:%F %T}", *tminfo);

    log->write("", LogType::Info, "Default model directory is " MODELS_DIR);
    log->write("", LogType::Info, "Default texture directory is " TEXTURES_DIR);
    log->write("", LogType::Info, "Default material directory is " MATERIALS_DIR);

    LoopRunner lr;

    graphics::Window* win = nullptr;
    GUIManager* guir      = nullptr;
    PlayerManager* pm     = nullptr;
    Game* g               = nullptr;

    try {
        {
            auto devs = std::move(pi.devices);
            std::unique_ptr<Device> defaultdev;

            if (devs.size() == 0) {
                log->write("main", LogType::Fatal, "no video devices found!");
                exit(1);
            }

            for (auto& d : devs) {
                log->write(
                    "main", LogType::Info, "driver found: {} {}", d->getCode().data(),
                    (d->isDefault() ? "(default)" : ""));

                if (d->isDefault()) {
                    defaultdev = std::move(d);
                }
            }

            if (!defaultdev) {
                log->write(
                    "main", LogType::Warning,
                    "no default device available, choosing the first one");
                defaultdev = std::move(devs[0]);
            }
            GFXService::setDevice(std::move(defaultdev));
        }

        auto& device = GFXService::getDevice();

        auto ipr = std::make_unique<InputProcessor>();
        InputService::setInputManager(std::make_unique<InputManager>(*ipr.get()));
        InputService::setCommandTable(std::make_unique<CommandTable>());

        {
            auto& ct = input::InputService::getCommandTable();
            ct->loadConfiguration({
                {"<up>", "CameraMove, up"},
                {"<down>", "CameraMove, down"},
                {"<right>", "CameraMove, right"},
                {"<left>", "CameraMove, left"},
                {"+", "CameraZoom, in"},
                {"-", "CameraZoom, out"},
                {"<kp-add>", "CameraZoom, in"},
                {"<kp-subtract>", "CameraZoom, out"},
                {"c", "DebugCreateEntity, tower"},
                {"e", "DebugCreateEntity, tent"},
                {"r", "DebugDestroyEntity"},
                {"b", "DebugShowBoundingBox"},
            });
        }

        auto& ima = InputService::getInputManager();

        GFXService::createTextureManager(
            std::make_unique<TextureManager>(device->createTextureEnv()));

        //        InputManager::GetInstance()->Initialize();
        win = device->createWindow(pi.width, pi.height);

        win->show();
        // enable_gl_debug();

        if (!init_network()) {
            throw net_exception("Could not initialize network");
        }

        log->write("", LogType::Info, "Device name: {}", device->getName().data());
        log->write("", LogType::Info, "Device vendor: {} ", device->getVendor().data());

        int fwidth, fheight;
        int gwidth, gheight;
        win->getFramebufferSize(fwidth, fheight);
        win->getFramebufferSize(gwidth, gheight);

        Framebuffer* f3D  = GFXService::getDevice()->createFramebuffer("f3D", fwidth, fheight);
        Framebuffer* fGUI = GFXService::getDevice()->createFramebuffer("fGUI", gwidth, gheight);
        win->setFramebuffers(f3D, fGUI);

        auto theme = std::make_unique<GUITheme>();
        theme->loadFile(ASSET_FILE_DIR "theme.yml");

        guir        = new GUIManager(win->createGUIRenderer());
        guir->theme = std::move(theme);
        guir->onResize(gwidth, gheight);

        auto& texman = GFXService::getTextureManager();

        /* If we have a networked game ready, don't even show the main menu. */
        auto [texw, texh] = texman->getTextureMaxSize();
        log->write("texture", LogType::Info, "maximum tex size: {} x {}", texw, texh);

        if (pi.mapFile || pi.inputFile) {
            int frames = 0;

            std::string mapFile = pi.inputFile ? "" : *pi.mapFile;

            auto createNormalSession_fn = [](logic::Terrain& map, auto& local_player_info) {
                /// running a normal game
                return initSinglePlayerSession(map, local_player_info);
            };

            Game* g = start_game(
                GraphicalInfo{f3D, fGUI, win, guir, (size_t)gwidth, (size_t)gheight}, lr,
                StartGameInfo{mapFile, pi.inputFile}, confdata, createNormalSession_fn);

            if (g) {
                lr.load([&]() { return g->runLoop(); });
                run_game_loop(lr, frames);
            }

            if (g) delete g;

            delete win;
            delete f3D;
            delete fGUI;
            fmt::print("\nExited. ({:d} frames)\n", frames);

        } else if (pi.serverAddress) {
            std::string addr = *pi.serverAddress;

            if (addr.find_first_of(':') == std::string::npos) {
                addr += ":8100";
            }

            fmt::print("Connecting to {}\n", addr);
            start_networked_game_cmdline(
                GraphicalInfo{f3D, fGUI, win, guir, (size_t)gwidth, (size_t)gheight}, addr,
                confdata);
        } else {
            std::exit(show_starting_menu(
                pi, GraphicalInfo{f3D, fGUI, win, guir, (size_t)gwidth, (size_t)gheight}, lr,
                confdata));
        }

    } catch (shader_exception& se) {
        log->write("init", LogType::Fatal, "Shader error: {}", se.what());

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
        log->write("init", LogType::Fatal, "Window creation error: {}", we.what());

        if (win) {
            std::string content = fmt::format("Graphical error: {}\n", we.what());
            win->showMessageBox("Familyline Error", SysMessageBoxFlags::Error, content);
        }

        exit(EXIT_FAILURE);
    } catch (logic_exception& le) {
        log->write("init", LogType::Fatal, "Logic exception: {}", le.what());

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
        log->write("init", LogType::Fatal, "Network exception: {}", le.what());

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
        log->write("init", LogType::Fatal, "Allocation error: {}", be.what());

        log->write("init", LogType::Fatal, "Probably out of memory");

        if (win) {
            std::string content = fmt::format("Insufficient memory\n\nError: {:s}", be.what());
            win->showMessageBox("Familyline Error", SysMessageBoxFlags::Error, content);
        }

        exit(EXIT_FAILURE);
    }

    end_network();
}

static int show_starting_menu(
    const ParamInfo& pi, GraphicalInfo ginfo, LoopRunner& lr, ConfigData& confdata)
{
    std::thread async_test;
    std::atomic<int> val    = 0;
    std::atomic<bool> aexit = false;

    auto sf = ServerFinder();

    GUIScriptRunner gsr(ginfo.guir);

    Game* g   = nullptr;
    auto& ima = InputService::getInputManager();
    /* Render the menu */
    bool r = true;

    auto& log = LoggerService::getLogger();

    gsr.registerPublicFunction("start-game", [&](s7_scheme* sc, s7_pointer args) -> s7_pointer {
        if (async_test.joinable()) {
            if (!aexit) {
                aexit = true;
                async_test.join();
            }
        }

        ginfo.guir->destroyWindow("bg");
        ginfo.guir->destroyWindow("menu");

        auto createNormalSession_fn = [](logic::Terrain& map, auto& local_player_info) {
            /// running a normal game
            return initSinglePlayerSession(map, local_player_info);
        };

        g = start_game(
            ginfo, lr, StartGameInfo{ASSET_FILE_DIR "terrain_test.flte", std::nullopt}, confdata,
            createNormalSession_fn);

        if (g) {
            lr.load([&]() { return g->runLoop(); });
        }

        return s7_t(sc);
    });

    gsr.registerPublicFunction("exit-game", [&](s7_scheme* sc, s7_pointer args) -> s7_pointer {
        r = false;
        return s7_t(sc);
    });

    gsr.registerPublicFunction("get-version", [&](s7_scheme* sc, s7_pointer args) -> s7_pointer {
        return s7_make_string(sc, VERSION);
    });

    gsr.registerPublicFunction("get-commit-id", [&](s7_scheme* sc, s7_pointer args) -> s7_pointer {
        return s7_make_string(sc, COMMIT);
    });

    gsr.registerPublicFunction(
        "get-config-option", [&](s7_scheme* sc, s7_pointer args) -> s7_pointer {
            auto property = ScriptEnvironment::convertTypeFrom<std::string>(sc, args);

            if (*property == "enable-input-recording")
                return s7_make_boolean(sc, confdata.enableInputRecording);
            if (*property == "player/username")
                return s7_make_string(sc, confdata.player.username.c_str());

            log->write("script-config", LogType::Error, "unknown property '{}'", *property);
            return s7_f(sc);
        });

    gsr.registerPublicFunction(
        "set-config-option", [&](s7_scheme* sc, s7_pointer args) -> s7_pointer {
            s7_pointer p  = s7_car(args);
            s7_pointer v  = s7_cdr(args);
            auto property = ScriptEnvironment::convertTypeFrom<std::string>(sc, p);

            if (*property == "enable-input-recording") {
                auto value = ScriptEnvironment::convertTypeFrom<bool>(sc, v);
                if (!value) {
                    log->write(
                        "script-config", LogType::Error, "unknown type for property '{}' ({})",
                        *property, std::make_pair(sc, v));
                    return s7_f(sc);
                }
                confdata.enableInputRecording = *value;
                return v;
            }
            if (*property == "player/username") {
                auto value = ScriptEnvironment::convertTypeFrom<std::string>(sc, v);
                if (!value) {
                    log->write(
                        "script-config", LogType::Error, "unknown type for property '{}' ({})",
                        *property, std::make_pair(sc, v));
                    return s7_f(sc);
                }
                confdata.player.username = *value;
                return v;
            }

            log->write("script-config", LogType::Error, "unknown property '{}'", *property);
            return s7_f(sc);
        });

    gsr.registerPublicFunction(
        "show-message-box", [&](s7_scheme* sc, s7_pointer args) -> s7_pointer {
            s7_pointer title = s7_car(args);
            s7_pointer msg   = s7_cdr(args);

            fmt::print(stderr, "AAA {}", std::make_pair(sc, args));

            auto stitle = ScriptEnvironment::convertTypeFrom<std::string>(sc, title);
            auto smsg   = ScriptEnvironment::convertTypeFrom<std::string>(sc, msg);
            if (!smsg || !stitle) {
                return s7_f(sc);
            }

            fprintf(stderr, "%s %s\n", stitle->c_str(), smsg->c_str());

            ginfo.win->showMessageBox(
                *stitle, SysMessageBoxFlags::Warning, fmt::format("{}", *smsg));
            return s7_t(sc);
        });

    bool is_discovering = false;
    gsr.registerPublicFunction(
        "multiplayer-listen-start", [&](s7_scheme* sc, s7_pointer args) -> s7_pointer {
            if (!s7_is_procedure(args)) {
                return s7_f(sc);
            }

            s7_pointer fptr = args;
            auto fguard     = s7_gc_protect(sc, fptr);

            sf.startDiscover([=](ServerInfo si) {
                fmt::print(
                    " \033[1m{}\033[0m ({}/{})\n{}:{}\n\n", si.name, si.player_count, si.player_max,
                    si.ip_addr, si.port);

                s7_call(
                    sc, fptr,
                    s7_list(
                        sc, 5, s7_make_string(sc, si.name.c_str()),
                        s7_make_string(sc, si.ip_addr.c_str()), s7_make_integer(sc, si.port),
                        s7_make_integer(sc, si.player_count), s7_make_integer(sc, si.player_max)));
            });

            is_discovering = true;
            return s7_immutable(s7_cons(
                sc, fptr,
                s7_make_c_pointer_with_type(
                    sc, (void*)fguard, s7_make_symbol(sc, "guard"), s7_t(sc))));
        });

    gsr.registerPublicFunction(
        "multiplayer-listen-stop", [&](s7_scheme* sc, s7_pointer args) -> s7_pointer {
            if (!s7_is_pair(args) || !s7_is_procedure(s7_car(args))) {
                return s7_f(sc);
            }

            if (!s7_is_c_pointer_of_type(s7_cdr(args), s7_make_symbol(sc, "guard"))) {
                return s7_f(sc);
            }

            if (!is_discovering) return s7_t(sc);

            s7_gc_unprotect_at(sc, (s7_int)s7_c_pointer(s7_cdr(args)));

            sf.stopDiscover();
            is_discovering = false;
            return s7_t(sc);
        });

    CServer cserv{};

    auto errHandler = [&](std::string addr, std::string msg, NetResult ret) {
        switch (ret) {
            case NetResult::ConnectionError:
                ginfo.win->showMessageBox(
                    msg, SysMessageBoxFlags::Warning,
                    fmt::format("Could not connect to address {}: Connection error", addr));
                return "connection-error";
            case NetResult::WrongPassword:
                ginfo.win->showMessageBox(
                    msg, SysMessageBoxFlags::Warning,
                    fmt::format("Server {} has a password", addr));
                return "wrong-password";
                break;
            case NetResult::LoginFailure:
                ginfo.win->showMessageBox(
                    msg, SysMessageBoxFlags::Warning,
                    fmt::format(
                        "Could not log in to {}\nThe server was found, but we "
                        "could not "
                        "login there.",
                        addr));
                return "login-failure";
            case NetResult::ConnectionTimeout:
                ginfo.win->showMessageBox(
                    msg, SysMessageBoxFlags::Warning,
                    fmt::format(
                        "Timed out while connecting to {}\nThe server probably "
                        "does not "
                        "exist, or there is a firewall issue.",
                        addr));
                return "connection-timeout";
            case NetResult::ServerError:
                ginfo.win->showMessageBox(
                    msg, SysMessageBoxFlags::Warning,
                    fmt::format(
                        "Error while connecting to {}. The server sent incorrect "
                        "data to "
                        "the client.",
                        addr));
                return "server-error";
            case NetResult::NotAllClientsConnected:
                ginfo.win->showMessageBox(
                    msg, SysMessageBoxFlags::Warning,
                    fmt::format(
                        "Error while connecting to {}. The server reported that "
                        "not all "
                        "clients were connected, but the client and the server "
                        "disagree on "
                        "that.",
                        addr));
                return "not-all-clients-connected";
            default:
                ginfo.win->showMessageBox(
                    msg, SysMessageBoxFlags::Warning,
                    fmt::format("Could not connect to address {}: unknown error {}", addr, ret));
                return "unknown-error";
        }
    };

    gsr.registerPublicFunction(
        "multiplayer-connect", [&](s7_scheme* sc, s7_pointer args) -> s7_pointer {
            auto addr = ScriptEnvironment::convertTypeFrom<std::string>(sc, args);
            if (!addr) {
                return s7_f(sc);
            }

            if (addr->size() < 3) return s7_f(sc);

            auto ret = cserv.login(*addr, confdata.player.username);
            if (ret != NetResult::OK) {
                auto res = errHandler(*addr, "Connection error", ret);
                return s7_list(
                    sc, 4, s7_make_symbol(sc, "login-info"), s7_f(sc),
                    s7_make_string(sc, addr->c_str()), s7_make_symbol(sc, res));
            }

            /// Return a list with a login info tag, a success
            /// tag, the login address and an opaque pointer to cserv
            return s7_immutable(s7_list(
                sc, 4, s7_make_symbol(sc, "login-info"), s7_t(sc),
                s7_make_string(sc, addr->c_str()),
                s7_make_c_pointer_with_type(
                    sc, &cserv, s7_make_symbol(sc, "login-info"), s7_t(sc))));
        });

    gsr.registerPublicFunction(
        "multiplayer-login", [&](s7_scheme* sc, s7_pointer args) -> s7_pointer {
            auto tag = s7_car(args);
            if (!s7_is_symbol(tag) || std::string{s7_symbol_name(tag)} != "login-info")
                return s7_f(sc);

            if (s7_list_length(sc, args) != 4) return s7_f(sc);

            if (!s7_is_eqv(sc, s7_t(sc), s7_cadr(args))) return s7_f(sc);

            auto addr = ScriptEnvironment::convertTypeFrom<std::string>(sc, s7_caddr(args));
            if (!addr) {
                return s7_f(sc);
            }

            auto sptr = s7_cadddr(args);
            if (!s7_is_c_pointer_of_type(sptr, s7_make_symbol(sc, "login-info"))) return s7_f(sc);
            CServer* receivecserv = (CServer*)s7_c_pointer(sptr);

            start_networked_game_room(
                ginfo, *receivecserv,
                std::bind(errHandler, *addr, std::placeholders::_1, std::placeholders::_2),
                confdata);
            cserv.logout();
            return s7_t(sc);
        });

    gsr.load(SCRIPTS_DIR "gui/gui.scm");

    // auto deflistener = InputManager::GetInstance()->GetDefaultListener();

    GUIWindow* w = gsr.openMainWindow();
    if (!w) {
        throw std::runtime_error("No valid menu window in script");
    }

    // TODO: Autoresize this when autoresizing the gui manager
    w->onResize(ginfo.gwidth - 2, ginfo.gheight - 2, 1, 1);

    GUIWindow& background = ginfo.guir->createWindow<FlexLayout<false>>("bg");
    background.onResize(ginfo.gwidth, ginfo.gheight, 0, 0);
    ginfo.guir->onResize(ginfo.gwidth, ginfo.gheight);

    ginfo.guir->showWindow(background);
    ginfo.guir->moveWindowToTop(*w);

    auto& texman            = GFXService::getTextureManager();
    TextureHandle th        = *texman->loadTexture(ICONS_DIR "/familyline-logo.png");
    GUIImageView& backimage = (GUIImageView&)background.box().add(
        ginfo.guir->createControl<GUIImageView>(*texman->getRawTexture(th)));

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

        ginfo.guir->runEvents();
        ginfo.guir->update();

        // Render
        ginfo.fGUI->startDraw();

        ginfo.guir->render();
        // guir->renderToScreen();
        ginfo.fGUI->endDraw();

        ginfo.win->update();

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

    delete ginfo.win;
    delete ginfo.f3D;
    delete ginfo.fGUI;
    fmt::print("\nExited. ({:d} frames)\n", frames);

    return 0;
}

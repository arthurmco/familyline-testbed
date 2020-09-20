/***
    Familyline main file

    Copyright 2016, 2017, 2019-2020 Arthur Mendes.

***/

#define GLM_FORCE_RADIANS
#include <GL/glew.h>

#ifdef __linux__
#include <GL/glxew.h>
#include <unistd.h>

#endif

#ifdef _WIN32

#define _WINSOCKAPI_
#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define usleep(x) Sleep(x / 1000);

#endif

#include <fmt/format.h>

#include <client/Game.hpp>
#include <client/graphical/device.hpp>
#include <client/graphical/framebuffer.hpp>
#include <client/graphical/gui/gui_button.hpp>
#include <client/graphical/gui/gui_imageview.hpp>
#include <client/graphical/gui/gui_label.hpp>
#include <client/graphical/gui/gui_manager.hpp>
#include <client/graphical/renderer.hpp>
#include <client/graphical/shader_manager.hpp>
#include <client/graphical/window.hpp>
#include <client/input/InputPicker.hpp>
#include <client/input/input_service.hpp>
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
 * This function allows us to run different functions
 * in the game loop, without the loop itself needing to
 * know it
 */
class LoopRunner
{
private:
    /**
     * The function we need to run
     *
     * If this function returns false, the loop must be stopped
     */
    std::function<bool()> fn_;

    /**
     * The function we loaded
     *
     * We first store them, and swap them before running, so
     * that we do not crash because we loaded a loop run
     * function in the middle of another loop run function
     */
    std::function<bool()> loadedfn_;

public:
    void load(std::function<bool()> fn) { loadedfn_ = fn; }

    bool run()
    {
        fn_ = loadedfn_;
        return fn_();
    }
};

static int show_starting_menu();

static int get_arg_index(const char* name, int argc, char const* argv[])
{
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], name)) {
            return i;
        }
    }

    return -1;
}

static void show_version()
{
    fmt::print("Familyline " VERSION "\n");
    fmt::print("Compiled in " __DATE__ "\n");
    fmt::print("Commit hash " COMMIT "\n");
    fmt::print("\n");
}

static void show_help()
{
    fmt::print("Familyline help:\n");
    fmt::print("--version:\t\tPrint version and, if compiled inside a Git repo, commit hash\n");
    fmt::print("--help:\t\t\tPrint this help information\n");
    fmt::print("--size <W>x<H>:\t\tChanges the game resolution to <W>x<H> pixels\n");
    fmt::print("--connect <ipaddr>:\tConnects to a game server whose IP is ipaddr\n");
    fmt::print(
        "--log [<filename>|screen]: Logs to filename 'filename', or screen to log to screen, or "
        "wherever stderr is bound to\n");
}

static int winW = 640, winH = 480;

static int check_size(int i, int argc, char const* argv[])
{
    if (i >= argc) {
        fmt::print("size not defined. Expected <W>x<H> for size! Aborting");
        return -1;
    }

    if (sscanf(argv[i + 1], "%dx%d", &winW, &winH) <= 1) {
        fmt::print("size format is wrong. Expected <W>x<H> for size! Aborting...");
        return -1;
    }

    fmt::print("pre-load: chosen {:d} x {:d} for screen size\n", winW, winH);
    return 0;
}

PlayerManager* pm = nullptr;

int main(int argc, char const* argv[])
{
    // Start log as early as possible

    FILE* fLog = nullptr;

    int i = get_arg_index("--log", argc, argv);
    if (i >= 0) {
        if ((i + 1) >= argc) {
            fmt::print(stderr, "--log: expected a filename or 'screen'\n");
            return EXIT_FAILURE;
        }

        fmt::print(stderr, "logging in {:s}\n", argv[i + 1]);
        if (!strcmp(argv[i + 1], "screen")) {
            fLog = stderr;
        } else {
            fLog = fopen(argv[i + 1], "a");
            fputs("\n\n", fLog);
        }
        if (!fLog) {
            perror("Could not create log file: ");
            fmt::print(stderr, "Using defaults");
        }
    }

    if (!fLog) {
        if (isatty(fileno(stderr))) {
            fLog = stderr;

#ifdef _WIN32
            // Set output mode to handle virtual terminal sequences, so the
            // color escape sequences can work on Windows
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
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
#endif

        } else {
            errno = 0;  // no need to worry
            /* Tries to create tribalia.log, fallback to stderr if it can't */
            fLog = fopen("tribalia.log", "a");
            fputs("\n\n", fLog);
            if (!fLog) {
                perror("Could not create log file: ");
                fmt::print(stderr, "Falling back to stderr");
                fLog = stderr;
            }
        }
    }

    LoggerService::createLogger(fLog, LogType::Info);

    // int winW = 640, winH = 480;
    if (get_arg_index("--version", argc, argv) >= 0) {
        show_version();
        return EXIT_SUCCESS;
    }

    i = get_arg_index("--size", argc, argv);
    if (i >= 0) {
        if (check_size(i, argc, argv) < 0) {
            return EXIT_FAILURE;
        }
    }

    if (get_arg_index("--help", argc, argv) >= 0) {
        show_help();
        return EXIT_SUCCESS;
    }

    auto& log = LoggerService::getLogger();

    log->write("", LogType::Info, "Familyline " VERSION);
    log->write("", LogType::Info, "built on " __DATE__ " by " USERNAME);
#if defined(COMMIT)
    log->write("", LogType::Info, "git commit is " COMMIT);
#endif

    char timestr[32];

    auto tm     = time(NULL);
    auto tminfo = localtime(&tm);
    strftime(timestr, 32, "%F %T", tminfo);
    log->write("", LogType::Info, "Actual date is %s", timestr);

    log->write("", LogType::Info, "Default model directory is " MODELS_DIR);
    log->write("", LogType::Info, "Default texture directory is " TEXTURES_DIR);
    log->write("", LogType::Info, "Default material directory is " MATERIALS_DIR);

    return show_starting_menu();
}

static void enable_gl_debug();

static int show_starting_menu()
{
    auto& log = LoggerService::getLogger();
    LoopRunner lr;

    graphics::Window* win = nullptr;
    GUIManager* guir      = nullptr;
    Game* g               = nullptr;
    try {
        auto devs          = graphics::getDeviceList();
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
        win = GFXService::getDevice()->createWindow(winW, winH);

        win->show();
        enable_gl_debug();

        log->write("", LogType::Info, "Device name: %s", defaultdev->getName().data());
        log->write("", LogType::Info, "Device vendor: %s ", defaultdev->getVendor().data());

        int fwidth, fheight;
        int gwidth, gheight;
        win->getFramebufferSize(fwidth, fheight);
        win->getFramebufferSize(gwidth, gheight);

        Framebuffer f3D  = Framebuffer("f3D", fwidth, fheight);
        Framebuffer fGUI = Framebuffer("fGUI", gwidth, gheight);
        win->setFramebuffers(&f3D, &fGUI);

        guir = new GUIManager(*win, (unsigned)gwidth, (unsigned)gheight, *ima.get());
        // guir->initShaders(win);

        /* If we have a networked game ready, don't even show the main menu. */
        log->write(
            "texture", LogType::Info, "maximum tex size: %zu x %zu", Texture::GetMaximumSize(),
            Texture::GetMaximumSize());

        /* Render the menu */
        bool r = true;
        // auto deflistener = InputManager::GetInstance()->GetDefaultListener();

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

        Button* bnew = new Button(400, 50, "New Game");  // Button(0.1, 0.2, 0.8, 0.1, "New Game");
        Button* bquit =
            new Button(400, 50, "Exit Game");  // Button(0.1, 0.31, 0.8, 0.1, "Exit Game");

        ImageView* ilogo = new ImageView(300, 450);  // 0.2, 0.1, 0.6, 0.9,
        ilogo->loadFromFile(ICONS_DIR "/familyline-logo.png");

        ilogo->z_index = -100;
        // ilogo.SetZIndex(0.9);
        // ilogo.SetOpacity(0.5);

        bquit->setClickCallback([&r](Control* cc) {
            (void)cc;
            r = false;
        });

        bnew->setClickCallback([&](Control* cc) {
            (void)cc;
            guir->remove(l);
            guir->remove(lv);
            guir->remove(bnew);
            guir->remove(bquit);
            guir->remove(ilogo);

            if (!pm) pm = new PlayerManager();

            g = new Game(win, &f3D, &fGUI, guir, pm);
            g->initLoopData();
            lr.load([&]() { return g->runLoop(); });
        });

        guir->add(0.37, 0.03, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)l));
        guir->add(0.32, 0.8, ControlPositioning::Relative, std::unique_ptr<Control>((Control*)lv));
        guir->add(0.1, 0.2, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)bnew));
        guir->add(0.1, 0.31, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)bquit));
        guir->add(0.2, 0.01, ControlPositioning::CenterX, std::unique_ptr<Control>((Control*)ilogo));

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
            fGUI.startDraw();
            guir->render(0, 0);
            // guir->renderToScreen();
            fGUI.endDraw();

            win->update();

            double e = SDL_GetTicks();
            if ((e - b) < 1000 / 60.0) SDL_Delay((unsigned int)(1000 / 60.0 - (e - b)));
            b = SDL_GetTicks();
            return r;
        });

        while (true) {
            if (!lr.run()) {
                break;
            }

            frames++;
        }

        if (g) delete g;
        if (pm) delete pm;

        delete guir;
        delete win;
        fmt::print("\nExited. ({:d} frames)\n", frames);

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

    return 0;
}

static void enable_gl_debug()
{
    auto& log = LoggerService::getLogger();
    struct LogTime {
        unsigned qt      = 0;
        unsigned lastsec = 0;
    };

    auto gl_debug_callback = [](GLuint source, GLuint type, unsigned int id, GLuint severity,
                                int length, const char* msg, const void* userparam) {
        (void)userparam;

        /* Handle log suppressing */
        static std::map<unsigned, LogTime> id_qt_map;
        static unsigned lastsupp = (unsigned)-1;
        auto t                   = time(NULL);
        auto& log                = LoggerService::getLogger();

        /* Unban after a long time */
        if (id == lastsupp && id_qt_map[id].lastsec < t + 5) {
            id_qt_map[id].qt = 0;
            return;
        } else {
            lastsupp = -1;
        }

        if (t >= id_qt_map[id].lastsec) {
            id_qt_map[id].qt++;
            id_qt_map[id].lastsec = t;
        }

        if (id_qt_map[id].qt > 20 && id_qt_map[id].lastsec <= t) {
            log->write(
                "gl-debug-output", LogType::Warning,
                "Suppressing id %d messages because they are too many", id);
            lastsupp = id;
            return;
        }

        id_qt_map[id].lastsec = t;

        /* Handle message parsing and display */
        const char *ssource, *stype, *sseverity;
        switch (source) {
            case GL_DEBUG_SOURCE_API: ssource = "gl-debug-opengl-api"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: ssource = "gl-debug-window-system"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: ssource = "gl-debug-shader-compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY: ssource = "gl-debug-third-party"; break;
            case GL_DEBUG_SOURCE_APPLICATION: ssource = "gl-debug-application"; break;
            case GL_DEBUG_SOURCE_OTHER: ssource = "gl-debug-other"; break;
            default: ssource = "gl-debug"; break;
        }

        switch (type) {
            case GL_DEBUG_TYPE_ERROR: stype = "error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: stype = "deprecated behavior"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: stype = "undefined behavior"; break;
            case GL_DEBUG_TYPE_PORTABILITY: stype = "portability issue"; break;
            case GL_DEBUG_TYPE_PERFORMANCE: stype = "performance"; break;
            case GL_DEBUG_TYPE_OTHER: stype = "other"; break;
            default: stype = "unknown"; break;
        }

        LogType ltype = LogType::Info;

        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                sseverity = "";
                ltype     = LogType::Error;
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                ltype     = LogType::Warning;
                sseverity = "";
                break;
            case GL_DEBUG_SEVERITY_LOW:
                ltype     = LogType::Debug;
                sseverity = " low prio: ";
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                ltype     = LogType::Debug;
                sseverity = "";
                break;
            default: sseverity = " PRIO: ????"; break;
        }

        char* smsg = new char[length + 2];
        memset(smsg, 0, length + 2);
        strncpy(smsg, msg, length);

        fmt::memory_buffer out;
        format_to(out, "[#{:d}] {:s} {:s}: {:s}\0\0", id, sseverity, stype, smsg);

        delete[] smsg;
        LoggerService::getLogger()->write(ssource, ltype, "%s", fmt::to_string(out).data());
    };

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    if (GL_KHR_debug && glDebugMessageCallback) {
        // Try KHR_debug first
        log->write("init", LogType::Info, "KHR_debug supported and used to get GPU debug messages");
        glDebugMessageCallback(gl_debug_callback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    } else if (GLEW_ARB_debug_output && glDebugMessageCallbackARB) {
        // Try ARB_debug_output as a fallback (it's older, but best supported)
        log->write(
            "init", LogType::Info,
            "ARB_debug_output supported and used to get GPU debugging messages");
        /* Create the callback */
        glDebugMessageCallbackARB(gl_debug_callback, nullptr);
        glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    } else {
        log->write(
            "init", LogType::Warning,
            "Neither KHR_debug nor ARB_debug_output extensions are supported");
        log->write("init", LogType::Warning, "GPU debugging messages will not be available");
    }
}

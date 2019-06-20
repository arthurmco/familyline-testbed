/***
    Familyline main file

    Copyright 2016, 2017 Arthur Mendes.

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
#define usleep(x) Sleep(x/1000);

#endif

#include "EnviroDefs.h"
#include "config.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <cstring>

#include <fmt/format.h>

#include "Game.hpp"

#include "graphical/renderer.hpp"
#include "graphical/shader_manager.hpp"
#include "graphical/window.hpp"
#include "graphical/framebuffer.hpp"

#include "net/NetServer.hpp"
#include "graphical/gui/GUIManager.hpp"
#include "graphical/gui/GUILabel.hpp"
#include "graphical/gui/GUIButton.hpp"
#include "graphical/gui/GUIImageControl.hpp"

#include "input/InputPicker.hpp"

#include "Log.hpp"

#include <glm/gtc/matrix_transform.hpp> //glm::lookAt()

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
#undef main  //somehow vs does not find main()

#endif

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
    fmt::print("--log [<filename>|screen]: Logs to filename 'filename', or screen to log to screen, or wherever stderr is bound to\n");
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

Net::Server* nserver = nullptr;
PlayerManager* pm = nullptr;
HumanPlayer* hp = nullptr;

int main(int argc, char const *argv[])
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
        }
        else {
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
            if (hOut == INVALID_HANDLE_VALUE)
            {
                return GetLastError();
            }

            DWORD dwMode = 0;
            if (!GetConsoleMode(hOut, &dwMode))
            {
                return GetLastError();
            }

            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (!SetConsoleMode(hOut, dwMode))
            {
                return GetLastError();
            }
#endif

        }
        else {
            errno = 0; // no need to worry
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
    Log::GetLog()->SetFile(fLog);

    //int winW = 640, winH = 480;
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


    Log::GetLog()->Write("", "Familyline " VERSION);
    Log::GetLog()->Write("", "built on " __DATE__ " by " USERNAME);
#if defined(COMMIT)
    Log::GetLog()->Write("", "git commit is " COMMIT);
#endif

    char timestr[32];

    auto tm = time(NULL);
    auto tminfo = localtime(&tm);
    strftime(timestr, 32, "%F %T", tminfo);
    Log::GetLog()->Write("", "Actual date is %s", timestr);

    Log::GetLog()->InfoWrite("", "Default model directory is " MODELS_DIR);
    Log::GetLog()->InfoWrite("", "Default texture directory is " TEXTURES_DIR);
    Log::GetLog()->InfoWrite("", "Default material directory is " MATERIALS_DIR);

    auto connectpos = get_arg_index("--connect", argc, argv);
    if (connectpos > 0) {
        if (connectpos >= argc - 1) {
            fmt::print(stderr, "--connect requires an argument: "
                "the server address\n\n");
            return EXIT_FAILURE;
        }

        const char* serveraddr = argv[connectpos + 1];
        fmt::print(" Connecting to {:s}...\n", serveraddr);

        try {
            nserver = new Net::Server(serveraddr);
            nserver->InitCommunications();

            fmt::print("Type the player's name: ");
            char pname[128];
            fgets(pname, 127, stdin);
            pname[strlen(pname) - 1] = '\0';

            // a npm that is good. (j/k :P)
            auto npm = nserver->GetPlayerManager(pname);
            hp = npm->GetHumanPlayer();
            npm->GetRemotePlayers(nserver);


            fmt::print("\n\tPress \033[1mENTER\033[0m to tell the server that you are \033[1mready\033[0m\n");

            char tmp[32];
            fgets(tmp, 31, stdin);
            nserver->SetReady(true);

            fmt::print("\n");
            char spwheel[] = { '\\', '|', '/', '-' };
            int i = 0;

            while (!nserver->IsGameStarting()) {
                nserver->GetMessages();
                usleep(250000);
                fmt::print("\r\tWaiting for other clients to be ready... "
                    "\033[37;1m{:c}\033[0m  ", spwheel[i % 4]);
                fflush(stdout);

                // Receive chats
                char mmsg[256];
                auto l = nserver->GetQueue()->PeekTCP(mmsg, 256);
                if (l > 0) {
                    char tr[10];
                    char ch[5];
                    int cid;
                    char cdest[16];
                    int mlen = 0;
                    char mcont[128];

                    if (sscanf(mmsg, "%s %s %d %s %d %s",
                        tr, ch, &cid, cdest, &mlen, mcont) == 6) {
                        if (!strcmp(tr, "[TRIBALIA") &&
                            !strcmp(ch, "CHAT")) {
                            nserver->GetQueue()->ReceiveTCP(mmsg, 256);
                            mcont[mlen] = 0;

                            fmt::print("[ID {:d}] {:s}\n", cid, mcont);

                        }
                    }


                }

                i++;
            }

            npm->GetMessageFilter()->SetServer(nserver);
            pm = (PlayerManager*)npm;

        }
        catch (Net::ServerException& e) {
            fmt::print(stderr, "Error while connecting to the server: {:s}\n",
                e.what());
            delete nserver;
            return EXIT_FAILURE;
        }

    }

    Framebuffer* fbGUI = nullptr;
    Framebuffer* fb3D = nullptr;
    graphics::Window* w = nullptr;
    GUIManager* guir = nullptr;
    try {
        w = new graphics::Window(winW, winH, WindowOptions::WIN_DEBUG_CONTEXT);
        Framebuffer::SetDefaultSize(winW, winH);
        w->Show();


        if (GLEW_ARB_debug_output && glDebugMessageCallbackARB) {
            struct LogTime {
                unsigned qt = 0;
                unsigned lastsec = 0;
            };

            auto gl_debug_callback = [](GLuint source, GLuint type,
                unsigned int id, GLuint severity,
                int length, const char* msg,
                const void* userparam) {
                (void)userparam;

                /* Handle log suppressing */
                static std::map<unsigned, LogTime> id_qt_map;
                static unsigned lastsupp = (unsigned)-1;
                auto t = time(NULL);

                /* Unban after a long time */
                if (id == lastsupp && id_qt_map[id].lastsec < t + 5) {
                    id_qt_map[id].qt = 0;
                    return;
                }
                else {
                    lastsupp = -1;
                }

                if (t >= id_qt_map[id].lastsec) {
                    id_qt_map[id].qt++;
                    id_qt_map[id].lastsec = t;
                }

                if (id_qt_map[id].qt > 20 && id_qt_map[id].lastsec <= t) {
                    Log::GetLog()->Write("gl-debug-output", "Suppressing id %d messages because they are too many", id);
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

                switch (severity) {
                case GL_DEBUG_SEVERITY_HIGH: sseverity = ""; break;
                case GL_DEBUG_SEVERITY_MEDIUM: sseverity = ""; break;
                case GL_DEBUG_SEVERITY_LOW: sseverity = " low prio: "; break;
                case GL_DEBUG_SEVERITY_NOTIFICATION: sseverity = ""; break;
                default: sseverity = " PRIO: ????"; break;
                }

                char* m = new char[std::max(length * 2, length + 70)];

                fmt::memory_buffer out;
                format_to(out, "[#{:d}]{:s} {:s}: {:s}",
                    id, sseverity, stype, msg);

                switch (severity) {
                case GL_DEBUG_SEVERITY_HIGH:
                    Log::GetLog()->Fatal(ssource, out.data());
                    break;
                case GL_DEBUG_SEVERITY_MEDIUM:
                    Log::GetLog()->Warning(ssource, out.data());
                    break;

                default:
                    Log::GetLog()->Write(ssource, out.data());
                    break;
                }

            };

            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

            if (GL_KHR_debug && glDebugMessageCallback) {
                // Try KHR_debug first
                Log::GetLog()->Write("init", "KHR_debug supported");
                glDebugMessageCallback(gl_debug_callback, nullptr);
                glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

            }
            else {
                // Try ARB_debug_output as a fallback (it's older, but best supported)
                Log::GetLog()->Write("init", "ARB_debug_output supported");

                /* Create the callback */
                glDebugMessageCallbackARB(gl_debug_callback, nullptr);
                glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
            }

        }
        else {
            Log::GetLog()->Warning("init", "ARB_debug_output not supported");
        }

        InputManager::GetInstance()->Initialize();

        fbGUI = new Framebuffer{ winW, winH, GL_UNSIGNED_BYTE };
        fb3D = new Framebuffer{ winW, winH, GL_UNSIGNED_BYTE };
        w->SetGUIFramebuffer(fbGUI);
        w->Set3DFramebuffer(fb3D);

        guir = new GUIManager{};
        guir->initShaders(w);


        /* If we have a networked game ready, don't even show the main menu. */
        if (nserver) {
            Log::GetLog()->Write("init", "Network game detected, going direct "
                "to it");

            //      guir->InitInput();
            auto g = Game(w, fb3D, fbGUI, guir, pm, hp);
            auto ret = g.RunLoop();
            if (pm)
                delete pm;
            delete hp;
            delete w;
            exit(ret);
        }

        Log::GetLog()->InfoWrite("texture", "maximum tex size: %zu x %zu", Texture::GetMaximumSize(),
            Texture::GetMaximumSize());


        /* Render the menu */
        bool r = true;
        auto deflistener = InputManager::GetInstance()->GetDefaultListener();
        int frames = 0;

        /* Lock frames to 60fps */
        double b = SDL_GetTicks();

        GUILabel l = GUILabel(0.37, 0.03, "FAMILYLINE");
        l.format.foreground = glm::vec4(1, 1, 1, 1);

        GUILabel lv = GUILabel(0.32, 0.8, "Version " VERSION ", commit " COMMIT);
        lv.format.foreground = glm::vec4(0.2, 0.2, 1, 1);
        lv.format.background = glm::vec4(1, 1, 1, 0.5);

        GUIButton bnew = GUIButton(0.1, 0.2, 0.8, 0.1, "New Game");
        GUIButton bquit = GUIButton(0.1, 0.31, 0.8, 0.1, "Exit Game");

        GUIImageControl ilogo = GUIImageControl(0.2, 0.1, 0.6, 0.9,
            ICONS_DIR "/tribalia-logo.png");
        ilogo.z_index = -100;
        //ilogo.SetZIndex(0.9);
        //ilogo.SetOpacity(0.5);

        bquit.onClickHandler = [&r](GUIControl* cc) {
            (void)cc;
            r = false;
        };

        bnew.onClickHandler = [&](GUIControl* cc) {
            (void)cc;
            guir->remove(&l);
            guir->remove(&lv);
            guir->remove(&bnew);
            guir->remove(&bquit);
            guir->remove(&ilogo);

            fmt::print("New Game\n");
            if (!pm)
                pm = new PlayerManager();

            if (!hp)
                hp = new HumanPlayer{ "Arthur", 0 };


            auto g = Game(w, fb3D, fbGUI, guir, pm, hp);
            auto ret = g.RunLoop();
            delete pm;
            delete hp;
            delete w;
            exit(ret);
        };

        guir->add(&l);
        guir->add(&lv);
        guir->add(&bquit);
        guir->add(&bnew);
        guir->add(&ilogo);

        while (r) {
            // Input
            InputManager::GetInstance()->Run();
            InputEvent ev;
            guir->update();

            if (deflistener->PopEvent(ev)) {
                /* Only listen for FINISH events.
                The others will be handled by the GUI listener */
                if (ev.eventType == EVENT_FINISH)
                    r = false;
            }

            // Render
            fbGUI->SetAsBoth();
            guir->render(0, 0);
            guir->renderToScreen();
            fbGUI->Unset();

            w->Update();
            double e = SDL_GetTicks();

            if ((e - b) < 1000 / 60.0)
                SDL_Delay((unsigned int)(1000 / 60.0 - (e - b)));

            b = SDL_GetTicks();

            frames++;
        }


        delete w;
        fmt::print("\nExited. ({:d} frames)\n", frames);

    }
    catch (window_exception& we) {
        Log::GetLog()->Fatal("init", "Window creation error: %s (%d)", we.what(), we.code);
        fmt::print(stderr, "Error while creating the window: {:s}\n", we.what());

        exit(EXIT_FAILURE);
    }
    catch (shader_exception &se) {
        Log::GetLog()->Fatal("init", "Shader error: %s [%d]", se.what(), se.code);
        Log::GetLog()->Fatal("init", "Shader file: %s, type %d", se.file.c_str(), se.type);

        if (w) {
            fmt::memory_buffer out;
            format_to(out,
                "Familyline found an error in a shader\n"
                "\n"
                "Error: {:s}\n"
                "File: {:s}, type: {:d}, code: {:d}",
                se.what(), se.file.c_str(), se.type, se.code);
            w->ShowMessageBox(out.data(), "Error", MessageBoxInfo::Error);
        }
        
        exit(EXIT_FAILURE);
    }
    catch (std::bad_alloc& be) {
        Log::GetLog()->Fatal("init", "Allocation error: %s", be.what());
        Log::GetLog()->Fatal("init", "Probably out of memory");

        if (w) {
            fmt::memory_buffer out;
            format_to(out,
                "Insufficient memory\n"
                "\n"
                "Error: {:s}",
                be.what());
            w->ShowMessageBox(out.data(), "Error", MessageBoxInfo::Error);
        }

        exit(EXIT_FAILURE);
    }

    return 0;
}

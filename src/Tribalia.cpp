/***
    Tribalia main file

    Copyright 2016, 2017 Arthur M.

***/

#define GLM_FORCE_RADIANS

#ifdef __linux__
#include <GL/glxew.h>
#include <unistd.h>

#endif

#include "EnviroDefs.h"
#include "config.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <cstring>

#include "Game.hpp"

#include "graphical/Renderer.hpp"
#include "graphical/GUIRenderer.hpp"
#include "graphical/ShaderProgram.hpp"
#include "graphical/Window.hpp"
#include "graphical/Framebuffer.hpp"
#include "graphical/AnimationManager.hpp"

#include "net/NetServer.hpp"
#include "graphical/gui/Panel.hpp"
#include "graphical/gui/Label.hpp"
#include "graphical/gui/Button.hpp"
#include "graphical/gui/ImageControl.hpp"

#include "input/InputPicker.hpp"

#include "Log.hpp"

#include <glm/gtc/matrix_transform.hpp> //glm::lookAt()

using namespace Tribalia;
using namespace Tribalia::Logic;
using namespace Tribalia::Graphics;
using namespace Tribalia::Graphics::GUI;
using namespace Tribalia::Input;

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
    printf("Tribalia " VERSION "\n");
    printf("Compiled in " __DATE__ "\n");
    printf("Commit hash " COMMIT "\n");
    printf("\n");
}

static void show_help()
{
	printf("Tribalia help:\n");
	printf("--version:\t\tPrint version and, if compiled inside a Git repo, commit hash\n");
	printf("--help:\t\t\tPrint this help information\n");
	printf("--size <W>x<H>:\t\tChanges the game resolution to <W>x<H> pixels\n");
	printf("--connect <ipaddr>:\tConnects to a game server whose IP is ipaddr\n");
	printf("--log [<filename>|screen]: Logs to filename 'filename', or screen to log to screen, or wherever stderr is bound to\n");
}

static int winW = 640, winH = 480;

static int check_size(int i, int argc, char const* argv[])
{
    if (i >= argc) {
	printf("size not defined. Expected <W>x<H> for size! Aborting");
	return -1;
    }
    
    if (sscanf(argv[i+1], "%dx%d", &winW, &winH) <= 1) {
	printf("size format is wrong. Expected <W>x<H> for size! Aborting...");
	return -1;
    }
    
    printf("pre-load: chosen %d x %d for screen size\n", winW, winH);
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
	    fprintf(stderr, "--log: expected a filename or 'screen'\n");
	    return EXIT_FAILURE;
	}

	fprintf(stderr, "logging in %s\n", argv[i+1]);
	if (!strcmp(argv[i+1], "screen")) {
	    fLog = stderr;
	} else {
	    fLog = fopen(argv[i+1], "a");
	    fputs("\n\n", fLog);
	}
	if (!fLog) {
	    perror("Could not create log file: ");
	    fprintf(stderr, "Using defaults");
	}
    }

    if (!fLog) {
	if (isatty(fileno(stderr))) {
	    fLog = stderr;
	} else {
	    errno = 0; // no need to worry
	    /* Tries to create tribalia.log, fallback to stderr if it can't */
	    fLog = fopen("tribalia.log", "a");
	    fputs("\n\n", fLog);
	    if (!fLog) {
		perror("Could not create log file: ");
		fprintf(stderr, "Falling back to stderr");
		fLog = stderr;
	    }
	}
    }
    Log::GetLog()->SetFile(fLog);
    
    //int winW = 640, winH = 480;
    if (get_arg_index("--version",argc,argv) >= 0) {
	show_version();
	return EXIT_SUCCESS;
    }

    i = get_arg_index("--size", argc, argv);
    if (i >= 0) {
	if( check_size(i, argc, argv) < 0) {
    	    return EXIT_FAILURE;
	}
    }

    if (get_arg_index("--help", argc, argv) >= 0) {
	show_help();
	return EXIT_SUCCESS;
    }

    auto connectpos = get_arg_index("--connect", argc, argv);
    if (connectpos > 0) {
	if (connectpos >= argc-1) {
	    fprintf(stderr, "--connect requires an argument: "
		    "the server address\n\n");
	    return EXIT_FAILURE;
	}

	const char* serveraddr = argv[connectpos+1];
	printf(" Connecting to %s...\n", serveraddr);

	try {
	    nserver = new Net::Server(serveraddr);
	    nserver->InitCommunications();

	    printf("Type the player's name: ");
	    char pname[128];
	    fgets(pname, 127, stdin);
	    pname[strlen(pname)-1] = '\0';

	    // a npm that is good. (j/k :P)
	    auto npm = nserver->GetPlayerManager(pname);
	    hp = npm->GetHumanPlayer();
	    pm = (PlayerManager*)npm;
	    
	} catch (Net::ServerException& e) {
	    fprintf(stderr, "Error while connecting to the server: %s\n",
		    e.what());
	    delete nserver;
	    return EXIT_FAILURE;
	}

    }

    Log::GetLog()->Write("", "Tribalia " VERSION);
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

    Framebuffer* fbGUI = nullptr;
    Framebuffer* fb3D = nullptr;
    Graphics::Window* w = nullptr;
    GUIRenderer* guir = nullptr;
    try {
	w = new Graphics::Window(winW, winH, WindowOptions::WIN_DEBUG_CONTEXT);
	Framebuffer::SetDefaultSize(winW, winH);
	w->Show();

	if (GLEW_ARB_debug_output && glDebugMessageCallbackARB) {
	    struct LogTime {
		unsigned qt = 0;
		unsigned lastsec = 0;
	    };
	    
		auto gl_debug_callback = [](GLuint source, GLuint type, unsigned int id, GLuint severity,
					    int length, const char* msg, const void* userparam) {
   		    (void)userparam;

		    /* Handle log suppressing */
		    static std::map<unsigned, LogTime> id_qt_map;
		    static unsigned lastsupp = (unsigned)-1;
		    auto t = time(NULL);

		    /* Unban after a long time */
		    if (id == lastsupp && id_qt_map[id].lastsec < t+5) {
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

		    sprintf(m, "[#%d]%s %s: %s",
			    id, sseverity, stype, msg);

		    auto l = strlen(m);
		    if (m[l-1] == '\n')
			m[l-1] = 0;

		    switch(severity) {
		    case GL_DEBUG_SEVERITY_HIGH:
			Log::GetLog()->Fatal(ssource, m);
			break;
		    case GL_DEBUG_SEVERITY_MEDIUM:
			Log::GetLog()->Warning(ssource, m);
			break;

		    default:
			Log::GetLog()->Write(ssource, m);
			break;
		    }
		    
		    delete[] m;

		};

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		
		if (GL_KHR_debug && glDebugMessageCallback) {
			// Try KHR_debug first
			Log::GetLog()->Write("init", "KHR_debug supported");
			glDebugMessageCallback(gl_debug_callback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

		} else {
			// Try ARB_debug_output as a fallback (it's older, but best supported)
			Log::GetLog()->Write("init", "ARB_debug_output supported");

			/* Create the callback */
			glDebugMessageCallbackARB(gl_debug_callback, nullptr);
			glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
		
	} else {
	    Log::GetLog()->Warning("init", "ARB_debug_output not supported");
	}

	InputManager::GetInstance()->Initialize();

	fbGUI = new Framebuffer{ winW, winH, GL_UNSIGNED_BYTE };
	fb3D = new Framebuffer{ winW, winH, GL_UNSIGNED_BYTE };
	w->SetGUIFramebuffer(fbGUI);
	w->Set3DFramebuffer(fb3D);

	guir = new GUIRenderer{w};
	guir->SetFramebuffer(fbGUI);

    } catch (window_exception& we) {
	Log::GetLog()->Fatal("init", "Window creation error: %s (%d)", we.what(), we.code);
	fprintf(stderr, "Error while creating the window: %s\n", we.what());
	
	exit(EXIT_FAILURE);
    } catch (shader_exception &se) {
	Log::GetLog()->Fatal("init", "Shader error: %s [%d]", se.what(), se.code);
	Log::GetLog()->Fatal("init", "Shader file: %s, type %d", se.file.c_str(), se.type);

	if (w) {
	    char err[512+strlen(se.what())];
	    sprintf(err,
		    "Tribalia found an error in a shader\n"
		    "\n"
		    "Error: %s\n"
		    "File: %s, type: %d, code: %d",
		    se.what(), se.file.c_str(), se.type, se.code);
	    w->ShowMessageBox(err, "Error", MessageBoxInfo::Error);
	}
	
	exit(EXIT_FAILURE);
    }

    

    /* Render the menu */
    bool r = true;
    auto deflistener = InputManager::GetInstance()->GetDefaultListener();
    int frames = 0;

    /* Lock frames to 60fps */
    double b = SDL_GetTicks();

    Label l = Label(0.37, 0.03, 0.25, 0.1, "TRIBALIA");
    l.SetForeColor(230, 240, 235, 255);
    l.SetBackColor(0, 0, 0, 1);
    l.SetFontData("Garamond", 32);
    
    Label lv = Label(0.32, 0.8, 0.4, 0.05, "Version " VERSION ", commit " COMMIT);
    lv.SetForeColor(255, 255, 255, 255);
    lv.SetBackColor(0, 0, 0, 192);
    
    Button bnew = Button(0.1, 0.2, 0.8, 0.1, "New Game");
    bnew.SetBackColor(212, 212, 212, 181);
    bnew.SetForeColor(255, 0, 0, 255);
    
    Button bquit = Button(0.1, 0.31, 0.8, 0.1, "Exit Game");
    bquit.SetBackColor(212, 212, 212, 181);
    bquit.SetForeColor(255, 0, 0, 255);
    
    ImageControl ilogo = ImageControl(0.2, 0.1, 0.6, 0.9,
				      "icons/tribalia-logo.png");
    ilogo.SetZIndex(0.9);
    ilogo.SetOpacity(0.5);
    
    bquit.SetOnClickListener([&r](GUI::IControl* cc) {
	    (void) cc;
	    r = false;
	});
    bnew.SetOnClickListener([&](GUI::IControl* cc) {
	    (void) cc;
	    guir->RemovePanel(&l);
	    guir->RemovePanel(&lv);
	    guir->RemovePanel(&bnew);
	    guir->RemovePanel(&bquit);
	    guir->RemovePanel(&ilogo);
	    
	    printf("New Game\n");
	    if (!pm)
		pm = new PlayerManager();

	    if (!hp)
		hp = new HumanPlayer{"Arthur", 0};
		
		
	    auto g = Game(w, fb3D, fbGUI, guir, pm, hp);
	    exit(g.RunLoop());
	});

    guir->AddPanel(&l);
    guir->AddPanel(&lv);
    guir->AddPanel(&bnew);
    guir->AddPanel(&bquit);
    guir->AddPanel(&ilogo);
    guir->InitInput();
    
    while (r) {
		// Input
		InputManager::GetInstance()->Run();
		InputEvent ev;
		guir->ProcessInput(ev);
	
		if (deflistener->PopEvent(ev)) {
			/* Only listen for FINISH events.
			   The others will be handled by the GUI listener */
			if (ev.eventType == EVENT_FINISH)
			r = false;
		}

		// Render
		fbGUI->SetAsBoth();
		guir->DebugWrite(20, 20, "Test");
		guir->Render();
		fbGUI->Unset();
	
		w->Update();
		double e = SDL_GetTicks();
	
		if ((e-b) < 1000/60.0)
		    SDL_Delay((unsigned int)(1000/60.0 - (e-b)));
	
		b = SDL_GetTicks();
	
		frames++;
    }

    
    
    printf("\nExited. (%d frames)\n", frames);

    return 0;
}


/***
    Tribalia main file

    Copyright 2016 Arthur M.

***/

#define GLM_FORCE_RADIANS

#include <GL/glxew.h>

#include "EnviroDefs.h"
#include "config.h"

#include <cstdio>
#include <cstdlib>

#include <cstring>

#include "Game.hpp"

#include "graphical/Renderer.hpp"
#include "graphical/GUIRenderer.hpp"
#include "graphical/ShaderProgram.hpp"
#include "graphical/Window.hpp"
#include "graphical/Framebuffer.hpp"
#include "graphical/AnimationManager.hpp"

#include "graphical/gui/Panel.hpp"
#include "graphical/gui/Label.hpp"
#include "graphical/gui/Button.hpp"

#include "input/InputPicker.hpp"

#include "Log.hpp"

#include <glm/gtc/matrix_transform.hpp> //glm::lookAt()

using namespace Tribalia;
using namespace Tribalia::Logic;
using namespace Tribalia::Graphics;
using namespace Tribalia::Graphics::GUI;
using namespace Tribalia::Input;

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
	printf("--version:\tPrint version and, if compiled inside a Git repo, commit hash\n");
	printf("--help:\t\tPrint this help information\n");
	printf("--size <W>x<H>: Changes the game resolution to <W>x<H> pixels\n");
}

/* Retrieves video RAM size
   Not much useful for game, but great for statistics and graphic configuration
   prediction */
static int get_video_ram_size() {
    int vram = -1;

    if (GLEW_NVX_gpu_memory_info) {
	//NVIDIA
	Log::GetLog()->Write("init", "GL_NVX_gpu_memory_info is supported. Probably running in a Nvidia GPU");
	glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &vram);
	vram /= 1024;
	
    } else if (GLEW_ATI_meminfo) {
	// ATI/AMD
	Log::GetLog()->Write("init", "GL_ATI_meminfo is supported. Probably running in a AMD GPU");
	Log::GetLog()->Warning("get-video-ram", "GL_ATI_meminfo only allows getting avaliable memory, the vram count might be incorrect");
	
	int freevbo[4], freetex[4], freerb[4];
	glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, freevbo);
	glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, freetex);
	glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, freerb);

	vram = (freevbo[0] + freetex[0] + freerb[0]) / 1024;
    } else if (GLXEW_MESA_query_renderer) {
	// Generic: MESA
	Log::GetLog()->Write("init", "GLX_MESA_query_renderer is supported");

	unsigned int uvram;
	glXQueryCurrentRendererIntegerMESA(GLX_RENDERER_VIDEO_MEMORY_MESA,
					   &uvram);

	vram = (int) uvram;
    }

    return vram;
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

int main(int argc, char const *argv[])
{
    //int winW = 640, winH = 480;
	if (get_arg_index("--version",argc,argv) >= 0) {
	    show_version();
	    return EXIT_SUCCESS;
	}

	int i =  get_arg_index("--size", argc, argv);
	if (i >= 0) {
	    if( check_size(i, argc, argv) < 0) {
		return EXIT_FAILURE;
	    }
	}

	if (get_arg_index("--help", argc, argv) >= 0) {
		show_help();
		return EXIT_SUCCESS;
	}

	
    FILE* fLog = stderr;// fopen("log.txt", "w");
    Log::GetLog()->SetFile(fLog);
    Log::GetLog()->Write("", "Tribalia " VERSION);
    Log::GetLog()->Write("", "built on " __DATE__ " by " USERNAME);
#if defined(COMMIT)
    Log::GetLog()->Write("", "git commit is " COMMIT);
#endif

    Log::GetLog()->Write("", "Default model directory is " MODELS_DIR);
    Log::GetLog()->Write("", "Default texture directory is " TEXTURES_DIR);
    Log::GetLog()->Write("", "Default material directory is " MATERIALS_DIR);

    Framebuffer* fbGUI = nullptr;
    Framebuffer* fb3D = nullptr;
    Graphics::Window* w = nullptr;
    GUIRenderer* guir = nullptr;
    try {
	w = new Graphics::Window(winW, winH, WindowOptions::WIN_DEBUG_CONTEXT);
	Framebuffer::SetDefaultSize(winW, winH);
	w->Show();

	if (GLEW_ARB_debug_output && glDebugMessageCallbackARB) {
		auto gl_debug_callback = [](GLuint source, GLuint type, unsigned int id, GLuint severity,
			int length, const char* msg, const void* userparam) {

			(void)userparam;

			const char *ssource, *stype, *sseverity;
			switch (source) {
			case GL_DEBUG_SOURCE_API: ssource = "OpenGL API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: ssource = "window system"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: ssource = "shader compiler"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY: ssource = "third party"; break;
			case GL_DEBUG_SOURCE_APPLICATION: ssource = "application"; break;
			case GL_DEBUG_SOURCE_OTHER: ssource = "other"; break;
			default: ssource = "unknown"; break;
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
			case GL_DEBUG_SEVERITY_HIGH: sseverity = "PRIO: HIGH"; break;
			case GL_DEBUG_SEVERITY_MEDIUM: sseverity = "PRIO: MEDIUM"; break;
			case GL_DEBUG_SEVERITY_LOW: sseverity = "PRIO: LOW"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: sseverity = ""; break;
			default: sseverity = "PRIO: ????"; break;
			}

			char* m = new char[std::max(length * 2, length + 70)];

			sprintf(m, "(%d) %s (source: %s | type: %s) %s",
				id, sseverity, ssource, stype, msg);

			if (severity == GL_DEBUG_SEVERITY_MEDIUM ||
				severity == GL_DEBUG_SEVERITY_HIGH) {
				Log::GetLog()->Warning("gl-debug-output", m);
			}
			else {
				Log::GetLog()->Write("gl-debug-output", m);
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

	Log::GetLog()->Write("", "Rendering Device Vendor: %s", glGetString(GL_VENDOR));
	Log::GetLog()->Write("", "Rendering Device Name: %s", glGetString(GL_RENDERER));

	auto maxtex = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtex);
	Log::GetLog()->Write("", "Maximum texture size: %dx%d", maxtex, maxtex);

	auto vram = get_video_ram_size();
	if (vram > 0) {
	    Log::GetLog()->Write("", "Video RAM size: %d MB", vram);
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
	exit(EXIT_FAILURE);
    } catch (shader_exception &se) {
	Log::GetLog()->Fatal("init", "Shader error: %s [%d]", se.what(), se.code);
	Log::GetLog()->Fatal("init", "Shader file: %s, type %d", se.file.c_str(), se.type);
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
    l.SetFontData("Garamond", 32);

    Label lv = Label(0.35, 0.8, 0.4, 0.1, "Version " VERSION ", commit " COMMIT);
    lv.SetForeColor(255, 255, 255, 255);
    
    Button bnew = Button(0.1, 0.2, 0.8, 0.1, "New Game");
    Button bquit = Button(0.1, 0.31, 0.8, 0.1, "Exit Game");
    bquit.SetOnClickListener([&r](GUI::IControl* cc) {
	    r = false;
	});
    bnew.SetOnClickListener([&](GUI::IControl* cc) {
	    guir->RemovePanel(&l);
	    guir->RemovePanel(&lv);
	    guir->RemovePanel(&bnew);
	    guir->RemovePanel(&bquit);
	    
	    printf("New Game\n");
	    auto g = Game(w, fb3D, fbGUI, guir);
	    exit(g.RunLoop());
	});    
    
    guir->AddPanel(&l);
    guir->AddPanel(&lv);
    guir->AddPanel(&bnew);
    guir->AddPanel(&bquit);
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

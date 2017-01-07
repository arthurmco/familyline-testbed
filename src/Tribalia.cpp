
/***
    Tribalia main file

    Copyright 2016 Arthur M.

***/

#define GLM_FORCE_RADIANS

#include "EnviroDefs.h"

#include <cstdio>
#include <cstdlib>

#include <glm/gtc/matrix_transform.hpp> //glm::lookAt()
#include <cstring>

#include "Log.hpp"
#include "Timer.hpp"
#include "HumanPlayer.hpp"
#include "logic/ObjectRenderer.hpp"
#include "logic/PathFinder.hpp"
#include "logic/ObjectFactory.hpp"
#include "logic/ObjectPathManager.hpp"
#include "logic/TerrainFile.hpp"

#include "graphical/Renderer.hpp"
#include "graphical/GUIRenderer.hpp"
#include "graphical/ShaderProgram.hpp"
#include "graphical/Camera.hpp"
#include "graphical/AssetFile.hpp"
#include "graphical/Light.hpp"
#include "graphical/meshopener/OBJOpener.hpp"
#include "graphical/materialopener/MTLOpener.hpp"
#include "graphical/TerrainRenderer.hpp"
#include "graphical/TextureOpener.hpp"
#include "graphical/TextureManager.hpp"
#include "graphical/MaterialManager.hpp"
#include "graphical/AssetManager.hpp"
#include "graphical/Window.hpp"
#include "graphical/Framebuffer.hpp"
#include "graphical/AnimationManager.hpp"

#include "graphical/gui/Panel.hpp"
#include "graphical/gui/Label.hpp"

#include "input/InputPicker.hpp"

#include "objects/WatchTower.hpp"
#include "objects/Tent.hpp"

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
	printf("--version:\tPrint version and, if compiled inside a Git repo, commit info\n");
	printf("--help:\t\tPrint this help information\n");
	printf("--size <W>x<H>: Changes the game resolution to <W>x<H> pixels\n");
}

int main(int argc, char const *argv[])
{
	int winW = 640, winH = 480;
	if (get_arg_index("--version",argc,argv) >= 0) {
		show_version();
		return EXIT_SUCCESS;
	}

	int i =  get_arg_index("--size", argc, argv);
	if (i >= 0) {
		if (i >= argc) {
			printf("size not defined. Expected <W>x<H> for size! Aborting...");
			return EXIT_FAILURE;
		}

		if (sscanf(argv[i+1], "%dx%d", &winW, &winH) <= 1) {
			printf("size format is wrong. Expected <W>x<H> for size! Aborting...");
			return EXIT_FAILURE;
		}

		printf("pre-load: chosen %d x %d for screen size\n", winW, winH);
	}

	if (get_arg_index("--help", argc, argv) >= 0) {
		show_help();
		return EXIT_SUCCESS;
	}


    FILE* fLog = stderr;// fopen("log.txt", "w");
    Log::GetLog()->SetFile(fLog);
    Log::GetLog()->Write("Tribalia " VERSION);
    Log::GetLog()->Write("built on " __DATE__ " by " USERNAME);
#if defined(COMMIT)
	Log::GetLog()->Write("git commit is " COMMIT);
#endif

    ObjectManager* om = nullptr;
    Window* win = nullptr;
    Renderer* rndr = nullptr;
    HumanPlayer* hp;
    SceneManager* scenemng;
    Terrain* terr;

    bool player = false;

    Camera* cam;


    AssetManager* am = AssetManager::GetInstance();
    Mesh* m;
    TerrainFile* terrFile;

    GameContext gctx;
    try {
        om = new ObjectManager{};
        win = new Window{ winW, winH };

        Framebuffer::SetDefaultSize(winW, winH);
        win->Show();

        rndr = new Renderer{};


        gctx.om = om;

        hp = new HumanPlayer{"Arthur"};
		terrFile = new TerrainFile("terrain_test.trtb");
		terr = terrFile->GetTerrain();
        scenemng = new SceneManager(terr->GetWidth() * SEC_SIZE, terr->GetHeight() * SEC_SIZE);

		cam = new Camera{glm::vec3(6.0f, 36.0f, 6.0f), (float)winW/(float)winH, glm::vec3(0,0,0)};
        scenemng->SetCamera(cam);
        hp->SetCamera(cam);

        rndr->SetSceneManager(scenemng);


        if (!am->ReadFromFile("assets.taif")) {
            throw asset_exception(nullptr, "Invalid asset file!");
        }

        m = am->GetAsset("Test2.obj")->asset.mesh;
        m->SetPosition(glm::vec3(4,1,4));
        m->GenerateBoundingBox();

    } catch (renderer_exception& re) {
        Log::GetLog()->Fatal("Rendering error: %s [%d]",
            re.what(), re.code);
        exit(EXIT_FAILURE);
    }  catch (mesh_exception& se) {
        Log::GetLog()->Fatal("Mesh error: %s",
            se.what());
        Log::GetLog()->Fatal("Mesh file: %s",
            se.file.c_str());
        exit(EXIT_FAILURE);
    }  catch (material_exception& se) {
        Log::GetLog()->Fatal("Material error: %s ",
            se.what());
        Log::GetLog()->Fatal("Material file: %s",
            se.file.c_str());
        exit(EXIT_FAILURE);
    } catch (shader_exception& se) {
        Log::GetLog()->Fatal("Shader error: %s [%d]",
            se.what(), se.code);
        Log::GetLog()->Fatal("Shader file: %s, type %d",
            se.file.c_str(), se.type);
        exit(EXIT_FAILURE);
    } catch (asset_exception& ae) {
        Log::GetLog()->Fatal("Asset file error: %s", ae.what());
        if (ae.assetptr) {
            AssetFileItem* a = (AssetFileItem*)ae.assetptr;
            Log::GetLog()->Fatal("Asset %s, file: %s", a->name.c_str(), a->path.c_str());
        }
        exit(EXIT_FAILURE);
    } catch (terrain_file_exception& te) {
		Log::GetLog()->Fatal("Terrain file error: %s on file %s", te.what(), te.file.c_str());
		if (te.code != 0) {
			Log::GetLog()->Fatal("Error code: %d (%s)", te.code, strerror(te.code));
		}
		exit(EXIT_FAILURE);
	}

	Texture* tex = am->GetAsset("test.bmp")->asset.texture;
	if (tex) {
		MaterialManager::GetInstance()->GetMaterial("Casa2")->SetTexture(tex);
        TextureManager::GetInstance()->AddTexture("test", tex);
    }

    Material* matest = new Material{"test", MaterialData(
        glm::vec3(0.0, 0.6, 0.7), glm::vec3(1.0, 0.9, 0.95), glm::vec3(0.1, 0.05, 0.06)
    )};
    MaterialManager::GetInstance()->AddMaterial(matest);

    MD2Opener op;
    Mesh* m5 = am->GetAsset("cabana.md2")->asset.mesh;
    m5->SetName("cabana");
    m5->SetPosition(glm::vec3(5, 1, 10));
    m5->SetRotation(glm::radians(-90.0f), 0, 0);
    m5->GenerateBoundingBox();
    m5->GetVertexData()->MaterialIDs.push_back(MaterialManager::GetInstance()->GetMaterial("test")->GetID());

    Mesh* m2 = am->GetAsset("Tent.obj")->asset.mesh;
    m2->SetPosition(glm::vec3(10, 1, 6));
    m2->SetRotation(0, glm::radians(-90.0f), 0);
    m2->GenerateBoundingBox();

	Mesh* m3 = am->GetAsset("testtex.obj")->asset.mesh;
	m3->SetPosition(glm::vec3(20, 1, 10));
	m3->GenerateBoundingBox();

	Light* l = new Light{ "mainLight", glm::vec3(16, 30, 16), 0xff, 0xff, 0xff, 10 };

    scenemng->AddObject(m);
    scenemng->AddObject(m2);
    scenemng->AddObject(m3);
    scenemng->AddObject(m5);
    scenemng->AddObject(l);

    TerrainRenderer* terr_rend = new TerrainRenderer{rndr};
    terr_rend->SetTerrain(terr);
    terr_rend->SetCamera(cam);

    ObjectRenderer* objrend = new ObjectRenderer(om, scenemng);
    hp->objr = objrend;

    InputManager::GetInstance()->Initialize();

    InputPicker* ip = new InputPicker{ terr_rend, win, scenemng, cam, om};
    hp->SetPicker(ip);

    PathFinder* pathf = new PathFinder(terr, om);
    pathf->UpdateSlotList(0, 0, terr->GetWidth(), terr->GetHeight());
    hp->SetPathfinder(pathf);

    unsigned int ticks = SDL_GetTicks();
    unsigned int frame = 0;

    int delta = 1;

	Framebuffer fbRender{ winW, winH, GL_UNSIGNED_BYTE };
	Framebuffer fbGUI{ winW, winH, GL_UNSIGNED_BYTE };
	win->Set3DFramebuffer(&fbRender);
	win->SetGUIFramebuffer(&fbGUI);

	GUIRenderer gr{ win };
	gr.SetFramebuffer(&fbGUI);

    Panel p = Panel{0, 0, 320, 240};
    p.SetBackColor(255, 0, 0, 25);
    gr.AddPanel(&p);

    Label lbl = Label(120, 460, "This is a true label");
    lbl.SetForeColor(255, 128, 0, 255);

    Label lblVersion = Label(10, 10, "");
    lblVersion.SetText("Tribalia " VERSION " commit " COMMIT);
    lblVersion.SetForeColor(255, 255, 255, 255);

    gr.AddPanel(&lbl);
    gr.AddPanel(&lblVersion);

	double pms = 0.0;

	/* Adds the objects to the factory */
	ObjectFactory::GetInstance()->AddObject(new WatchTower);
	ObjectFactory::GetInstance()->AddObject(new Tent);
    do {

        ip->UpdateIntersectedObject();
        ip->UpdateTerrainProjectedPosition();

		gr.DebugWrite(10, 40, "Press C to create an object at mouse cursor, and R to remove it.");
        player = true;
        gctx.elapsed_seconds = delta / 1000.0;

        if (!hp->Play(&gctx))
            player = false;

		terr_rend->Update();

        bool objupdate = objrend->Check();
        if (objupdate || hp->HasUpdatedObject()) {
            objrend->Update();
            pathf->UpdateSlotList(0, 0, terr->GetWidth(), terr->GetHeight());
        }
		objrend->Update();

        auto locc = ip->GetIntersectedObject();
        if (locc) {
            gr.DebugWrite(10, 100, "Hovering object '%s'", locc->GetName());
        }

        {
            int qx, qy;
            scenemng->GetCameraQuadrant(qx, qy);
            gr.DebugWrite(10, 160, "Camera quadrant: %d x %d", qx, qy);
        }

        LocatableObject* selected = hp->GetSelectedObject();
        if (selected) {
            gr.DebugWrite(10, 120, "Selected object: '%s'",
                selected->GetName());
        }


        glm::vec3 p = ip->GetTerrainProjectedPosition();
        glm::vec2 q = ip->GetGameProjectedPosition();

        AnimationManager::GetInstance()->Iterate();
		ObjectPathManager::getInstance()->UpdatePaths();

        gr.DebugWrite(10, 140, "Terrain pos: (OpenGL: %.3f,%.3f,%.3f | Game: %.2f, %.2f)",
             p.x, p.y, p.z, q.x, q.y);
		gr.DebugWrite(10, 180, "Camera rotation: %.1fº",
						cam->GetRotation() * 180 / M_PI);
        gr.DebugWrite(10, 65, "Bounding box: %s", hp->renderBBs ?
          "Enabled" : "Disabled");

		gr.Render();

		fbRender.SetActive();
		rndr->SetBoundingBox(hp->renderBBs);
		if (objupdate) rndr->UpdateObjects();
        rndr->UpdateFrames();
        rndr->Render();
		fbRender.UnsetActive();
		win->Update();

        frame++;

        unsigned int elapsed = SDL_GetTicks();
        delta = elapsed - ticks;

        ticks = SDL_GetTicks();
        Timer::getInstance()->RunTimers(delta);

		if (frame % 15 == 0) {
			pms = delta * 1.0;
		}

		gr.DebugWrite(0, 420, "%.2f ms, %.2f fps", pms, 1000 / pms);

    //Locked in ~60 fps
        if (delta < 1000/60.0) {
              SDL_Delay(1000/60.0 - delta);
        }

    } while (player);

    printf("\nExited. (%d frames) \n", frame);

    return 0;
}

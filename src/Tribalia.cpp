
/***
    Tribalia main file

    Copyright 2016 Arthur M.

***/

#define GLM_FORCE_RADIANS

#define COMMIT

#include "EnviroDefs.h"

#include <cstdio>
#include <cstdlib>

#include <glm/gtc/matrix_transform.hpp> //glm::lookAt()

#include "Log.hpp"
#include "HumanPlayer.hpp"
#include "logic/ObjectRenderer.hpp"
#include "logic/PathFinder.hpp"

#include "graphical/Renderer.hpp"
#include "graphical/GUIRenderer.hpp"
#include "graphical/ShaderProgram.hpp"
#include "graphical/Camera.hpp"
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

#include "input/InputPicker.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;
using namespace Tribalia::Graphics;
using namespace Tribalia::Graphics::GUI;
using namespace Tribalia::Input;

#ifdef _MSC_VER
    #undef main  //somehow vs does not find main()

    /* TODO: fixes these variables on win32 builds */
     //#define VERSION VERSION"-win32"
    #define COMMIT 0
    #define USERNAME "unknown"
#endif

int main(int argc, char const *argv[]) {
    FILE* fLog = stderr;// fopen("log.txt", "w");
    Log::GetLog()->SetFile(fLog);
    Log::GetLog()->Write("Tribalia %s", VERSION);
    Log::GetLog()->Write("built on %s by %s ", __DATE__, USERNAME);

    if (COMMIT > 0)
        Log::GetLog()->Write("from commit %07x", COMMIT);

    ObjectManager* om = nullptr;
    Window* win = nullptr;
    Renderer* rndr = nullptr;
    HumanPlayer* hp;
    SceneManager* scenemng;

    bool player = false;

    Camera* cam;

    AssetManager* am = AssetManager::GetInstance();
    Mesh* m;

    GameContext gctx;
    try {
      om = new ObjectManager{};
		  win = new Window{ 640, 480 };

		  Framebuffer::SetDefaultSize(640, 480);
		  win->Show();

      rndr = new Renderer{};


      gctx.om = om;

      hp = new HumanPlayer{"Arthur"};
      scenemng = new SceneManager();

      cam = new Camera{glm::vec3(6.0f, 24.0f, 6.0f), glm::vec3(0,0,0)};
      scenemng->SetCamera(cam);
      hp->SetCamera(cam);

      rndr->SetSceneManager(scenemng);


      am->ReadFromFile("test.taif");

      m = am->GetAsset("test2.obj")->asset.mesh;
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

#ifdef WIN32
		system("pause");
#endif
        exit(EXIT_FAILURE);
    }

	Texture* tex = am->GetAsset("test.bmp")->asset.texture;
	if (tex) {
		MaterialManager::GetInstance()->GetMaterial("Casa2")->SetTexture(tex);
        TextureManager::GetInstance()->AddTexture("test", tex);
    }

    Material* matest = new Material{"test", MaterialData(
        glm::vec3(1.0, 0.6, 0.7), glm::vec3(1.0, 0.9, 0.95), glm::vec3(0.1, 0.05, 0.06)
    )};
    MaterialManager::GetInstance()->AddMaterial(matest);

    MD2Opener op;
    Mesh* m5 = op.Open("cabana.md2");
    m5->SetName("cabana");
    m5->SetPosition(glm::vec3(5, 1, 10));
    m5->SetRotation(glm::radians(-90.0f), 0, 0);
    m5->GenerateBoundingBox();
    m5->GetVertexData()->MaterialIDs.push_back(MaterialManager::GetInstance()->GetMaterial("test")->GetID());

    Mesh* m2 = am->GetAsset("models/Tent.obj")->asset.mesh;
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

    Terrain* terr = new Terrain{1000, 1000};;
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

    int i = 0;
    unsigned int ticks = SDL_GetTicks();
    unsigned int frame = 0;

    int delta = 1;

	Framebuffer fbRender{ 640, 480, GL_UNSIGNED_BYTE };
	Framebuffer fbGUI{ 640, 480, GL_UNSIGNED_BYTE };
	win->Set3DFramebuffer(&fbRender);
	win->SetGUIFramebuffer(&fbGUI);

	GUIRenderer gr{ win };
	gr.SetFramebuffer(&fbGUI);

    gr.AddPanel(new Panel{0, 0, 320, 240});

	double pms = 0.0;
	int pframe = 0;


    do {

        ip->UpdateIntersectedObject();
        ip->UpdateTerrainProjectedPosition();

		gr.DebugWrite(10, 15, "Tribalia v0.0.1 Build 1, commit %07x", COMMIT);
		gr.DebugWrite(10, 35, "Press C to create an object at mouse cursor.");
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

        auto locc = ip->GetIntersectedObject();
        if (locc) {
            gr.DebugWrite(10, 100, "Hovering object '%s'", locc->GetName());
        }

        LocatableObject* selected = hp->GetSelectedObject();
        if (selected) {
            gr.DebugWrite(10, 120, "Selected object: '%s'",
                selected->GetName());
        }


        glm::vec3 p = ip->GetTerrainProjectedPosition();

        AnimationManager::GetInstance()->Iterate();

        gr.DebugWrite(10, 140, "Terrain pos: %.3f,%.3f,%.3f", p.x, p.y, p.z);
        gr.DebugWrite(10, 65, "Bounding box: %s", hp->renderBBs ?
          "Enabled" : "Disabled");

    	gr.Render();

    	fbRender.SetActive();
    	rndr->SetBoundingBox(hp->renderBBs);
		if (objupdate) rndr->UpdateObjects();
        rndr->Render();
    	fbRender.UnsetActive();
    	win->Update();

        frame++;

        unsigned int elapsed = SDL_GetTicks();
        delta = elapsed - ticks;

        ticks = SDL_GetTicks();

    	if (frame % 30 == 0) {
	    	pms = delta * 1.0;
    	}

	    gr.DebugWrite(0, 420, "%.1f ms, %.2f fps", pms, 1000 / pms);


	//glm::vec3 cur_wor = ip->GetCursorWorldRay();
	//printf("Cursor ray: (%.2f, %.2f %.2f)\t",
	//	cur_wor.x, cur_wor.y, cur_wor.z);
	//ip->GetTerrainProjectedPosition();

    //Trava em ~60 fps
        if (delta < 16) {
              SDL_Delay(16 - delta);
        }

        //usleep(1);

    } while (player);

    printf("\nExited. (%d frames) \n", frame);

    return 0;
}


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
#include "logic/HumanPlayer.hpp"
#include "logic/ObjectRenderer.hpp"

#include "graphical/Renderer.hpp"
#include "graphical/ShaderProgram.hpp"
#include "graphical/Camera.hpp"
#include "graphical/Light.hpp"
#include "graphical/meshopener/OBJOpener.hpp"
#include "graphical/materialopener/MTLOpener.hpp"
#include "graphical/Terrain.hpp"
#include "graphical/TerrainRenderer.hpp"
#include "graphical/TextureOpener.hpp"
#include "graphical/TextureManager.hpp"
#include "graphical/MaterialManager.hpp"
#include "graphical/AssetManager.hpp"

#include "input/InputPicker.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;
using namespace Tribalia::Graphics;
using namespace Tribalia::Input;

#ifdef _MSC_VER
    #undef main  //somehow vs does not find main()

    /* TODO: fixes these variables on win32 builds */
    #define VERSION "v0.0.1-win32"
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
    Renderer* rndr = nullptr;

    try {
        om = new ObjectManager{};
        rndr = new Renderer{};
    } catch (renderer_exception& re) {
        Log::GetLog()->Fatal("Rendering error: %s [%d]",
            re.what(), re.code);
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


    GameContext gctx;
    gctx.om = om;

    HumanPlayer hp = HumanPlayer{"Arthur"};
    SceneManager* scenemng = new SceneManager();
    

    bool player = false;

    Camera cam = Camera{glm::vec3(4.2f, 16.0f, 3.8f), glm::vec3(0,0,0)};
    scenemng->SetCamera(&cam);
    hp.SetCamera(&cam);

    rndr->SetSceneManager(scenemng);

    AssetManager am;
    am.ReadFromFile("test.taif");

    Mesh* m = am.GetAsset("test2.obj")->asset.mesh;
    m->SetPosition(glm::vec3(4,1,4));
    m->GenerateBoundingBox();

	
	Texture* tex = am.GetAsset("test.bmp")->asset.texture;
	if (tex) {
		MaterialManager::GetInstance()->GetMaterial("Casa2")->SetTexture(tex);
        TextureManager::GetInstance()->AddTexture("test", tex);
    }
	

    Mesh* m2 = am.GetAsset("casinha.obj")->asset.mesh;
    m2->SetPosition(glm::vec3(10, 1, 6));
    m2->SetRotation(0, glm::radians(-90.0f), 0);
    m2->GenerateBoundingBox();

	Mesh* m3 = am.GetAsset("testtex.obj")->asset.mesh;
	m3->SetPosition(glm::vec3(20, 1, 10));
	m3->GenerateBoundingBox();

	Light* l = new Light{ "mainLight", glm::vec3(16, 30, 6), 0xff, 0xff, 0xff, 20 };
	Light* l2 = new Light{ "mainLight2", glm::vec3(8, 5, 6), 0, 0, 255, 40 };
	Light* l3 = new Light{ "mainLight2", glm::vec3(16, 5, 6), 255, 0, 255, 40 };
	

    scenemng->AddObject(m);
    scenemng->AddObject(m2);
	scenemng->AddObject(m3);
	scenemng->AddObject(l);
	scenemng->AddObject(l2);
	scenemng->AddObject(l3);

    Terrain* terr = new Terrain{1000, 1000};
    TerrainRenderer* terr_rend = new TerrainRenderer{rndr};
    terr_rend->SetTerrain(terr);
    terr_rend->SetCamera(&cam);

    ObjectRenderer* objrend = new ObjectRenderer(om, scenemng);
    hp.objr = objrend;

	InputManager::GetInstance()->Initialize();

	InputPicker* ip = new InputPicker{ terr_rend, rndr, scenemng, &cam };

    int i = 0;
    unsigned int ticks = SDL_GetTicks();
    unsigned int frame = 0;
    int delta = 1;


    printf("==== \n Game launched\n");
    printf(" [C] - Create an object\n");
    printf("\n");

    do {
        player = true;
        gctx.elapsed_seconds = delta / 1000.0;

        if (!hp.Play(&gctx))
            player = false;

        terr_rend->Update();

        objrend->Check();
        objrend->Update();

        rndr->Render();
        frame++;

        unsigned int elapsed = SDL_GetTicks();
        delta = elapsed - ticks;

        ticks = SDL_GetTicks();

        printf("\033[1m %4d ms \033[0m\r", delta);

		glm::vec3 cur_wor = ip->GetCursorWorldRay();
		printf("Cursor ray: (%.2f, %.2f %.2f)\t",
			cur_wor.x, cur_wor.y, cur_wor.z);
		ip->GetTerrainProjectedPosition();

        //Trava em ~60 fps
        if (delta < 16) {
            SDL_Delay(16 - delta);
        }

        //usleep(1);
    } while (player);

    printf("\nExited. (%d frames) \n", frame);

    return 0;
}

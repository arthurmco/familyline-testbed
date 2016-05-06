
/***
    Tribalia main file

    Copyright 2016 Arthur M.

***/

#define GLM_FORCE_RADIANS

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
#include "graphical/meshopener/OBJOpener.hpp"
#include "graphical/Terrain.hpp"
#include "graphical/TerrainRenderer.hpp"
#include "graphical/MaterialManager.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;
using namespace Tribalia::Graphics;
using namespace Tribalia::Input;

int main(int argc, char const *argv[]) {
    Log::GetLog()->SetFile(stdout);
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
        exit(EXIT_FAILURE);
    }


    GameContext gctx;
    gctx.om = om;

    HumanPlayer hp = HumanPlayer{"Arthur"};
    SceneManager* scenemng = new SceneManager();
    InputManager* inputmng = new InputManager();
    hp.SetInputManager(inputmng);

    bool player = false;

    Camera cam = Camera{glm::vec3(0.0f, 16.0f, 16.0f), glm::vec3(0,0,0)};
    scenemng->SetCamera(&cam);
    hp.SetCamera(&cam);

    rndr->SetSceneManager(scenemng);

    OBJOpener opener;
    Mesh* m = opener.Open("test2.obj");
    m->SetPosition(glm::vec3(4,1,4));
    scenemng->AddObject(m);

    Terrain* terr = new Terrain{1000, 1000};
    TerrainRenderer* terr_rend = new TerrainRenderer{rndr};
    terr_rend->SetTerrain(terr);
    terr_rend->SetCamera(&cam);

    ObjectRenderer* objrend = new ObjectRenderer(om, scenemng);

    MaterialData matdata;
    matdata.diffuseColor = glm::vec3(0.6, 0.1, 0.0);
    matdata.diffuseIntensity = 0.6;
    matdata.ambientColor = glm::vec3(0.05, 0.0, 0.0);
    matdata.ambientIntensity = 0.1;
    Material mat = Material(0, "Test", matdata);
    MaterialManager::GetInstance()->AddMaterial(&mat);

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


        //Trava em ~60 fps
        if (delta < 16) {
            SDL_Delay(16 - delta);
        }

        //usleep(1);
    } while (player);

    printf("\nExited. (%d frames) \n", frame);

    return 0;
}

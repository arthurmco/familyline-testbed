
/***
    Tribalia main file

    Copyright 2016 Arthur M.

***/

#define GLM_FORCE_RADIANS

#include <cstdio>
#include <cstdlib>

#include <glm/gtc/matrix_transform.hpp> //glm::lookAt()

#include "Log.hpp"
#include "logic/HumanPlayer.hpp"

#include "graphical/Renderer.hpp"
#include "graphical/ShaderProgram.hpp"
#include "graphical/Camera.hpp"

#include "EnviroDefs.h"

#include "graphical/meshopener/OBJOpener.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;
using namespace Tribalia::Graphics;

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

    bool player = false;
    SDL_Event ev;

    Camera cam = Camera{glm::vec3(8.0f, 6.0f, 6.0f), glm::vec3(0,0,0)};
    scenemng->SetCamera(&cam);

    rndr->SetSceneManager(scenemng);

    OBJOpener opener;
    Mesh* m = opener.Open("test.obj");
    Mesh* m2 = opener.Open("test2.obj");
    Mesh* m3 = opener.Open("test.obj");

    if (!m || !m2) {
        printf(" Mesh não encontrada");
        return EXIT_FAILURE;
    }

    scenemng->AddObject(m);
    scenemng->AddObject(m2);
    scenemng->AddObject(m3);

    m->AddPosition(glm::vec3(3.0, 1.0, 1.0));
    m->ApplyTransformations();

    m2->AddPosition(glm::vec3(-3.0, 0.0, 0.0));
    m2->ApplyTransformations();
    m3->AddRotation(glm::radians(60.0f), 0, 0);
    m3->AddPosition(glm::vec3(0.0, -1.2, 2.5));
    m3->ApplyTransformations();

    int i = 0;
    do {
        player = true;

        if (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_WINDOWEVENT &&
                ev.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    player = false;
                }
        }

        rndr->Render();

        usleep(1);
    } while (player);

    printf("Exited.\n");

    return 0;
}

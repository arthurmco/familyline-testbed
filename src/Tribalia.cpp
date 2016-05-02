
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

#include "graphical/Renderer.hpp"
#include "graphical/ShaderProgram.hpp"
#include "graphical/Camera.hpp"
#include "graphical/meshopener/OBJOpener.hpp"

#include "input/InputManager.hpp"

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

    bool player = false;

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
    unsigned int ticks = SDL_GetTicks();
    InputEvent ev;
    bool front = false, back = false;
    bool left = false, right = false;
    do {
        player = true;
        inputmng->Run();

        if (inputmng->GetEvent(&ev)) {
            if (ev.eventType == EVENT_FINISH) {
                player = false;
            }

            if (ev.eventType == EVENT_KEYEVENT) {
                switch (ev.event.keyev.scancode) {
                    case SDLK_w:
                        if (ev.event.keyev.status == KEY_KEYPRESS)
                            front = true;
                        else if (ev.event.keyev.status == KEY_KEYRELEASE)
                            front = false;
                    break;
                    case SDLK_s:
                        if (ev.event.keyev.status == KEY_KEYPRESS)
                            back = true;
                        else if (ev.event.keyev.status == KEY_KEYRELEASE)
                            back = false;
                    break;
                    case SDLK_a:
                        if (ev.event.keyev.status == KEY_KEYPRESS)
                            left = true;
                        else if (ev.event.keyev.status == KEY_KEYRELEASE)
                            left = false;
                    break;
                    case SDLK_d:
                        if (ev.event.keyev.status == KEY_KEYPRESS)
                            right = true;
                        else if (ev.event.keyev.status == KEY_KEYRELEASE)
                            right = false;
                    break;
                }

            }

            inputmng->PopEvent(NULL);
        }

        if (front)
            cam.AddMovement(glm::vec3(0, 0, -0.009f));
        else if (back)
            cam.AddMovement(glm::vec3(0, 0, 0.009f));

        if (left)
            cam.AddMovement(glm::vec3(-0.009f, 0, 0));
        else if (right)
            cam.AddMovement(glm::vec3(0.009f, 0, 0));


        rndr->Render();

        unsigned int elapsed = SDL_GetTicks();
        int delta = elapsed - ticks;

        ticks = SDL_GetTicks();
        printf("\033[1m %4d ms \033[0m\r", delta);

        //Trava em ~60 fps
        if (delta < 16) {
            SDL_Delay(16 - delta);
        }

        //usleep(1);
    } while (player);

    printf("\nExited.\n");

    return 0;
}

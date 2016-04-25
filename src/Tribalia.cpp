
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
    }

    Shader *sFrag, *sVert;
    try {
        sFrag = new Shader{"shaders/Forward.frag", SHADER_PIXEL};
        sVert = new Shader{"shaders/Forward.vert", SHADER_VERTEX};
    } catch (shader_exception& se) {
        Log::GetLog()->Fatal("Shader error: %s [%d]",
            se.what(), se.code);
        Log::GetLog()->Fatal("Shader file: %s, type %d",
            se.file.c_str(), se.type);
        exit(EXIT_FAILURE);
    }

    if (!sFrag->Compile()) {
        Log::GetLog()->Fatal("Shader %s failed to compile", sFrag->GetPath());
        exit(EXIT_FAILURE);
    }

    if (!sVert->Compile()) {
        Log::GetLog()->Fatal("Shader %s failed to compile", sVert->GetPath());
        exit(EXIT_FAILURE);
    }

    ShaderProgram* sProg = new ShaderProgram{sVert, sFrag};
    if (!sProg->Link()) {
        Log::GetLog()->Fatal("Shader %d failed to link", sProg->GetID());
        exit(EXIT_FAILURE);
    }

    GameContext gctx;
    gctx.om = om;

    HumanPlayer hp = HumanPlayer{"Arthur"};

    bool player = false;
    SDL_Event ev;

    Camera cam = Camera{glm::vec3(6.0f, 1.0f, 2.5f), glm::vec3(0,0,0)};

    glm::mat4 mproj = cam.GetProjectionMatrix();
    glm::mat4 mview = cam.GetViewMatrix();

    glm::mat4 mmodel = glm::mat4(1.0f);
    glm::mat4 mvp = mproj * mview * mmodel;

    sProg->Use();
    sProg->SetUniform("color", glm::vec3(0.7, 0.9, 0.0));
    sProg->SetUniform("mvp", mvp);
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

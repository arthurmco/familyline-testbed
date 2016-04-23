#include "Renderer.hpp"

using namespace Tribalia::Graphics;

Renderer::Renderer()
{
    int sdl_ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    if (sdl_ret != 0) {
        /* Video hasn't been initialized correctly */
        char err[1024] = "SDL wasn't initialized correctly: ";
        strcat(err, SDL_GetError());
        throw renderer_exception(err, sdl_ret);
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,    8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,  8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    _win = SDL_CreateWindow("Tribalia", 0, 0, 640, 480,
        SDL_WINDOW_OPENGL);
    if (!_win) {
        char err[1024] = "Window creation error: ";
        strcat(err, SDL_GetError());
        SDL_Quit();
        throw renderer_exception(err, -10);
    }

    _glctxt = SDL_GL_CreateContext(_win);

    if (_glctxt == NULL) {
        char err[1024] = "OpenGL context creation error: ";
        strcat(err, SDL_GetError());
        SDL_Quit();
        throw renderer_exception(err, -11);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


}

/* Returns true if rendered successfully */
bool Renderer::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SDL_GL_SwapWindow(_win);
    return true;
}


Renderer::~Renderer()
{
    SDL_Quit();
}

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

    _win = SDL_CreateWindow("Tribalia", 0, 0, 640, 480,
        SDL_WINDOW_OPENGL);
    if (!_win) {
        char err[1024] = "Window creation error: ";
        strcat(err, SDL_GetError());
        SDL_Quit();
        throw renderer_exception(err, sdl_ret);
    }

    



}


bool Renderer::Render()
{
    return true;
}


Renderer::~Renderer()
{
    SDL_Quit();
}

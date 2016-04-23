/***
    Tribalia 3D renderer

    Copyright (C) 2016 Arthur M.

***/

#include <SDL2/SDL.h>
#include "GFXExceptions.hpp"

#include "../Log.hpp"

#ifndef RENDERER_HPP
#define RENDERER_HPP

namespace Tribalia {
namespace Graphics {

    class Renderer
    {
    private:
        SDL_Window* _win;


    public:
        Renderer();
        ~Renderer();

        bool Render();

    };

}
}


#endif /* end of include guard: RENDERER_HPP */

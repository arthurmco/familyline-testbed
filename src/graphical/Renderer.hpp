/***
    Tribalia 3D renderer

    Copyright (C) 2016 Arthur M.

***/

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <vector>

#include "GFXExceptions.hpp"

#include "VertexData.hpp"
#include "../Log.hpp"

#ifndef RENDERER_HPP
#define RENDERER_HPP

namespace Tribalia {
namespace Graphics {

    struct VertexRenderInfo {
        VertexData* vd;
        glm::mat4 worldMat;
    };

    class Renderer
    {
    private:
        SDL_Window* _win;
        SDL_GLContext _glctxt;

        std::vector<VertexRenderInfo> _vertices;
    public:
        Renderer();
        ~Renderer();

        /* Returns true if rendered successfully */
        bool Render();

        /* Add vertex data structure. Returns its VAO ID */
        GLint AddVertexData(VertexData*, glm::mat4 worldMatrix);
    };

}
}


#endif /* end of include guard: RENDERER_HPP */

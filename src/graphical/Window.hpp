/*  Window management class
    Also works on joining the GUI with the general game renderer.

    Copyright (C) 2016 Arthur M
*/

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "ShaderProgram.hpp"
#include "GFXExceptions.hpp"

#ifndef WINDOW_HPP
#define WINDOW_HPP

namespace Tribalia {
namespace Graphics {

    class Window {
    private:
        int _width, _height;
        SDL_Window* _win;
        SDL_GLContext _glctxt;

		ShaderProgram* winShader = nullptr;

        GLuint base_vao, base_vbo, base_index_vbo;
    public:
        Window(int w, int h);

        /* Shows the window */
        void Show();

        /* Updates the window content to the video card */
        void Update();
    };


} /* Graphics */
} /* Tribalia */


#endif /* end of include guard: WINDOW_HPP */

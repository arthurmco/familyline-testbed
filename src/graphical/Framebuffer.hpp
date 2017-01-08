/***
    Framebuffer wrapping class

    Copyright (C) 2016,2017 Arthur M
***/

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "../Log.hpp"
#include "GFXExceptions.hpp"

#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

namespace Tribalia {
namespace Graphics {

    class Framebuffer {
    private:
        GLuint fb_handle;
        GLuint tex_handle;
        GLuint renderbuffer_handle;

        int _width, _height;

		static int defWidth, defHeight;

    public:
        Framebuffer(int w, int h, GLenum format);

        /* Set this framebuffer to be the active one */
        void SetActive();

        /*  Remove this framebuffer from the active state and
            bring back the default one */
        void UnsetActive();

        /*  Get the texture handle for this framebuffer
            Useful when you want to use its content as a texture */
        GLint GetTextureHandle();
	
	static void SetDefaultSize(int w, int h) {
	    defWidth = w; defHeight = h;
	}
    };


} /* Graphics */
} /* Tribalia */

#endif /* end of include guard: FRAMEBUFFER_HPP */

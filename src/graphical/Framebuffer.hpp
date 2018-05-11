/***
    Framebuffer wrapping class

    Copyright (C) 2016,2017 Arthur M
***/

#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <GL/glew.h>

#include <SDL2/SDL_opengl.h>

#include "Log.hpp"
#include "GFXExceptions.hpp"

namespace Familyline {
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

        /* Set this framebuffer to be the read framebuffer */
        void SetAsRead();

	/* Set the framebuffer to be the write framebuffer */
	void SetAsWrite();

	/* Set the framebuffer as both read and write */
	void SetAsBoth();

	/* Set the framebuffer as nothing */
	void Unset();
	
        /*  Get the texture handle for this framebuffer
            Useful when you want to use its content as a texture */
        GLint GetTextureHandle();
	
	static void SetDefaultSize(int w, int h) {
	    defWidth = w; defHeight = h;
	}
    };


} /* Graphics */
} /* Familyline */

#endif /* end of include guard: FRAMEBUFFER_HPP */

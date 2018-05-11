#include "Framebuffer.hpp"

using namespace Familyline::Graphics;

/*	Create the framebuffer, its texture (for getting the image) and its
	renderbuffer (to correctly render depth) */
Framebuffer::Framebuffer(int w, int h, GLenum format)
{
    _width = w;
    _height = h;

    glGenFramebuffers(1, &fb_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_handle);

    glGenTextures(1, &tex_handle);
    glBindTexture(GL_TEXTURE_2D, tex_handle);

    GLenum glerr = GL_NO_ERROR;
    if ((glerr = glGetError()) != GL_NO_ERROR) {
	Log::GetLog()->Warning("framebuffer",
			       "GL error %#x while binding texture handle %d",
			       glerr, tex_handle);
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, format, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, &renderbuffer_handle);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_handle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_handle);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_handle, 0);

    GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, draw_buffers);

    GLenum fb_error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fb_error != GL_FRAMEBUFFER_COMPLETE)
	throw new renderer_exception{ "The framebuffer is not complete",
		(int)fb_error };

    Log::GetLog()->Write("framebuffer", "Created framebuffer id %#x texid %#x "
			 "with %d x %d", fb_handle, tex_handle, w, h);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/* Set this framebuffer to be the active one */
void Framebuffer::SetAsRead()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_handle);
    glViewport(0, 0, _width, _height);
}

void Framebuffer::SetAsBoth()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fb_handle);
//    glViewport(0, 0, _width, _height);
}

void Framebuffer::SetAsWrite()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb_handle);
    glViewport(0, 0, _width, _height);
}

int Framebuffer::defWidth, Framebuffer::defHeight;

/*  Remove this framebuffer from the active state and
    bring back the default one */
void Framebuffer::Unset()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glViewport(0, 0, defWidth, defHeight);
}


/*  Get the texture handle for this framebuffer
    Useful when you want to use its content as a texture */
GLint Framebuffer::GetTextureHandle()
{
    return tex_handle;
}

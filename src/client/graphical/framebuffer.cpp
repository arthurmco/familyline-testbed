#include <client/graphical/framebuffer.hpp>
#include <client/graphical/exceptions.hpp>
#include <common/Log.hpp>

using namespace familyline::graphics;

Framebuffer::Framebuffer(std::string_view name, int width, int height)
	: _name(name)
{
	glGenFramebuffers(1, &_handle);
	glBindFramebuffer(GL_FRAMEBUFFER, _handle);

	auto l = Log::GetLog();
	l->InfoWrite("fb", "creating framebuffer %s: handle=%#x, size=%d x %d",
		name.data(), _handle, width, height);

	glGenRenderbuffers(1, &_rboHandle);
	glBindRenderbuffer(GL_RENDERBUFFER, _rboHandle);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rboHandle);
    

	this->setupTexture(width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureHandle, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		l->Warning("fb", "framebuffer %s (%#x) is not complete", name.data(), _handle); // should be error
	}

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		char e[128];
		snprintf(e, 127, "error %#x while creating framebuffer %s",
			err, name.data());
		throw graphical_exception(std::string(e));
	}

	l->InfoWrite("fb", "\tcompleted: texhandle=%#x, rbohandle=%#x",
		_textureHandle, _rboHandle);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/// Setup the texture bound to the framebuffer
///
/// Each framebuffer has a texture attached to it. All rendering commands sent to the framebuffer will write
/// to the texture, and the texture will have the final rendered 2d image
void Framebuffer::setupTexture(int width, int height)
{
	glGenTextures(1, &_textureHandle);
	glBindTexture(GL_TEXTURE_2D, _textureHandle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::startDraw()
{
    glBindRenderbuffer(GL_RENDERBUFFER, _rboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, _handle);
}

void Framebuffer::endDraw()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


int Framebuffer::getTextureHandle()
{
	return _textureHandle;
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &_handle);
}

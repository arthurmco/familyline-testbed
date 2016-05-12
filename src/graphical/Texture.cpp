#include "Texture.hpp"

using namespace Tribalia::Graphics;

Texture::Texture(int width, int height, GLenum format, void* data)
{
    glGenTextures(1, &_tex_handle);
    glBindTexture(GL_TEXTURE_2D, _tex_handle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    Log::GetLog()->Write("Created texture with %dx%d, format 0x%#x, with id %d",
        width, height, format, _tex_handle);
}

GLint Texture::GetHandle() const { return _tex_handle; }

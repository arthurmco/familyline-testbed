#include "Texture.hpp"

using namespace Tribalia::Graphics;

Texture::Texture(int width, int height, GLenum format, void* data)
{
    GLuint tex_handle = 0;
    
    // Create, bind and setup texture
    glGenTextures(1, &tex_handle);
    glBindTexture(GL_TEXTURE_2D, _tex_handle);

    GLenum dest_format = GL_RGB;
    if (format == GL_RGBA) {
	dest_format = GL_RGBA;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, dest_format, width, height, 0, format,
		 GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    Log::GetLog()->Write("Created texture with %dx%d, format 0x%#x, with id %d",
        width, height, format, _tex_handle);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
    _tex_handle = tex_handle;
}

GLint Texture::GetHandle() const { return _tex_handle; }


void Texture::SetName(const char* name)
{
    _name = std::string{name};
}

const char* Texture::GetName() const
{
    return _name.c_str();
}


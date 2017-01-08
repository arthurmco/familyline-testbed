/***
    Texture representation class

    Copyright (C) 2016,2017 Arthur M

***/
#include <SDL2/SDL_opengl.h>

#include <string>
#include "../Log.hpp"

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

namespace Tribalia {
namespace Graphics {

class Texture
{
private:
    GLuint _tex_handle;
    std::string _name;
    
public:
    Texture(int width, int height, GLenum format, void* data);

    GLint GetHandle() const;
    
    void SetName(const char* name);
    const char* GetName() const;
};

} /* Graphics */
} /* Tribalia */

#endif /* end of include guard: TEXTURE_HPP */

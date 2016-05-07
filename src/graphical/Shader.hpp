/***
    Shader abstraction

    Copyright (C) 2016 Arthur M

***/

#include <string>
#include <cstring> //strcat
#include <GL/glew.h>

#include <cstdio>

//for stat the file, to discover its size
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include "../Log.hpp"

#include "GFXExceptions.hpp"

#ifndef SHADER_HPP
#define SHADER_HPP

namespace Tribalia {
namespace Graphics {

    enum ShaderType
    {
        SHADER_VERTEX,
        SHADER_PIXEL,
        SHADER_GEOMETRY,
        SHADER_PROGRAM
    };

    class Shader
    {
    private:
        GLint _id;
        std::string _path;
        int _type;

    public:
        GLint GetID() const;
        const char* GetPath() const;
        int GetType() const;

        Shader(const char* path, int type);
        bool Compile();
    };

}
}



#endif /* end of include guard: SHADER_HPP */

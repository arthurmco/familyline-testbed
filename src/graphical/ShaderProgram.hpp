/***
    Shader program abstraction

    Copyright (C) 2016 Arthur M

***/

#include "Shader.hpp"

#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

namespace Tribalia {
namespace Graphics {

    class ShaderProgram
    {
    private:
        Shader* _vertex;
        Shader* _pixel;

        GLint _id;
    public:
        ShaderProgram(Shader* vert, Shader* pixel);
        bool Link();
        void Use();

        GLint GetID();

    };

}
}


#endif /* end of include guard: SHADERPROGRAM_HPP */

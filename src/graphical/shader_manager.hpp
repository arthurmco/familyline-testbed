#pragma once

#include "shader.hpp"
#include <memory>
#include <map>

namespace familyline::graphics {

    class ShaderManager {
    private:
        int lastShader = -1;

        std::map<std::string, ShaderProgram*> _shaders;

    public:
        void use(ShaderProgram& s);
        void addShader(ShaderProgram* s);
        ShaderProgram* getShader(std::string_view name);
    };

}

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
        /**
         * Use the shader, i.e, sets it as the active one in the videocard
         *
         * Note that you need to use the shader before setting uniforms,
         * or you will get a bunch of invalid operation errors!
         */
        void use(ShaderProgram& s);
        
        void addShader(ShaderProgram* s);
        ShaderProgram* getShader(std::string_view name);
    };

}

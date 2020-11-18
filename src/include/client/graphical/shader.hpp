#pragma once

#if !(defined(__gl_h_) || defined(__GL_H__) || defined(_GL_H) || defined(__X_GL_H))
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>

enum ShaderType { Vertex, Fragment, Geometry, Compute };

#include <string>
#include <string_view>

namespace familyline::graphics
{
/**
 * Represents a shader file
 *
 * We unite those shader files in a shader program.
 * @see ShaderProgram
 */
class Shader
{
protected:
    const char* _file;
    ShaderType _type;

public:
    Shader(const char* file, ShaderType type)
        : _file(file), _type(type)
        {}
        
    virtual void compile() = 0;

    virtual int getHandle() const = 0;
    
    ShaderType getType() const { return this->_type; }

    virtual ~Shader() {}
};

class ShaderProgram
{
protected:
    std::string_view _name;
public:
    ShaderProgram(std::string_view name)
        : _name(name)
        {}
    
    virtual void link() = 0;

    virtual int getHandle() const = 0;
    
    std::string_view getName() const { return _name; }

    virtual void setUniform(std::string_view name, glm::vec3 val) = 0;
    virtual void setUniform(std::string_view name, glm::vec4 val) = 0;
    virtual void setUniform(std::string_view name, glm::mat4 val) = 0;
    virtual void setUniform(std::string_view name, int val) = 0;
    virtual void setUniform(std::string_view name, float val) = 0;

    virtual ~ShaderProgram() {}

    virtual void use() = 0;
};
}  // namespace familyline::graphics

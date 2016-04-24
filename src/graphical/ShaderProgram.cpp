#include "ShaderProgram.hpp"

using namespace Tribalia::Graphics;

ShaderProgram::ShaderProgram(Shader* vert, Shader* pixel)
{
    this->_id = glCreateProgram();

    this->_vertex = vert;
    this->_pixel = pixel;

    glAttachShader(this->_id, vert->GetID());
    glAttachShader(this->_id, pixel->GetID());

    Log::GetLog()->Write("Shader program with id %d created", this->_id);
    Log::GetLog()->Write("Shader program %d contains shaders %s and %s",
        this->_id, vert->GetPath(), pixel->GetPath());
}
bool ShaderProgram::Link()
{
    glLinkProgram(this->_id);

    GLint res = GL_FALSE;
    glGetProgramiv(this->_id, GL_LINK_STATUS, &res);

    /* Retrieve shader logs anyway */
    GLint logsize = 0;
    glGetProgramiv(this->_id, GL_INFO_LOG_LENGTH, &logsize);

    if (logsize > 1) {
        char logdata[logsize];

        glGetProgramInfoLog(this->_id, logsize, NULL, logdata);
        Log::GetLog()->Warning("Shader program %d retrieved a log: \n%s",
            this->_id, logdata);
    }

    return (res == GL_TRUE);

}

void ShaderProgram::Use()
{
    glUseProgram(this->_id);
}

GLint ShaderProgram::GetID()
{
    return this->_id;
}

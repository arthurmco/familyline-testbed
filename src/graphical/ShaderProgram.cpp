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


bool ShaderProgram::SetUniform(const char* name, glm::mat4 value)
{
    GLint unif_id = glGetUniformLocation(this->_id, name);
    glUniformMatrix4fv(unif_id, 1, GL_FALSE, (const GLfloat*)&value[0][0]);
    Log::GetLog()->Write("Added uniform %s (id %d) to shader %d",
        name, unif_id, _id);
    return (unif_id > 0);
}

bool ShaderProgram::SetUniform(const char* name, glm::vec3 value)
{
    GLint unif_id = glGetUniformLocation(this->_id, name);
    glUniform3fv(unif_id, 1, (const GLfloat*) &value[0]);
    Log::GetLog()->Write("Setted uniform %s (id %d) to (%.3f %.3f %.3f) on shader %d",
        name, unif_id, value.x, value.y, value.z, _id);
    return (unif_id > 0);
}

GLint ShaderProgram::GetID()
{
    return this->_id;
}

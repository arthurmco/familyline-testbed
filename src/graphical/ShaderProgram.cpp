#include "ShaderProgram.hpp"

using namespace Tribalia::Graphics;

ShaderProgram::ShaderProgram(Shader* vert, Shader* pixel)
{
    this->_id = glCreateProgram();

    this->_vertex = vert;
    this->_pixel = pixel;

    glAttachShader(this->_id, vert->GetID());
    glAttachShader(this->_id, pixel->GetID());

    Log::GetLog()->Write("shader-program", "Shader program with id %d created",
			 this->_id);
    Log::GetLog()->Write("shader-program", "Shader program %d contains shaders %s and %s",
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
        char* logdata = new char[logsize];

        glGetProgramInfoLog(this->_id, logsize, NULL, logdata);
        Log::GetLog()->Warning("shader-program", "Shader program %d retrieved a log: \n%s",
            this->_id, logdata);

	if (res == GL_TRUE)
	    throw shader_exception(logdata, 0, "", SHADER_PROGRAM);

    }

    return (res == GL_TRUE);

}

void ShaderProgram::Use()
{
    glUseProgram(this->_id);
}


/* Tries to get the uniform location.
First, it query the cache, then it asks for OpenGL*/
GLint ShaderProgram::GetUniformLocation(const char* name)
{
	std::string sname{ name };
	if (uniform_cache.find(sname) == uniform_cache.end()) {
		// Not found. Adds to the cache
		uniform_cache[sname] = glGetUniformLocation(this->_id, name);
	}

	return uniform_cache[sname];
}


bool ShaderProgram::SetUniform(const char* name, glm::mat4 value)
{
    GLint unif_id = GetUniformLocation(name);

    if (unif_id < 0) {
        Log::GetLog()->Write("shader-program",
			     "Uniform %s not found on shader %d", name, _id);
    }

    glUniformMatrix4fv(unif_id, 1, GL_FALSE, (const GLfloat*)&value[0][0]);
    //Log::GetLog()->Write("Setted uniform %s (id %d) on shader %d",
    //        name, unif_id, _id);
    return (unif_id > 0);
}

bool ShaderProgram::SetUniform(const char* name, glm::vec3 value)
{
    GLint unif_id = GetUniformLocation(name);
    glUniform3fv(unif_id, 1, (const GLfloat*) &value[0]);
    //Log::GetLog()->Write("Setted uniform %s (id %d) to (%.3f %.3f %.3f) on shader %d",
    //    name, unif_id, value.x, value.y, value.z, _id);
    return (unif_id > 0);
}

bool ShaderProgram::SetUniform(const char* name, float value)
{
    GLint unif_id = GetUniformLocation(name);
    glUniform1f(unif_id, value);

    //Log::GetLog()->Write("Setted uniform %s (id %d) to (%.3f %.3f %.3f) on shader %d",
    //    name, unif_id, value.x, value.y, value.z, _id);
    return (unif_id > 0);
}

bool ShaderProgram::SetUniform(const char* name, int value)
{
	GLint unif_id = GetUniformLocation(name);
	glUniform1i(unif_id, value);

	if (unif_id < 0) {
		Log::GetLog()->Write("shader-program",
				     "Uniform %s not found on shader %d",
			name, _id);
	}
	
	//Log::GetLog()->Write("Setted uniform %s (id %d) to (%.3f %.3f %.3f) on shader %d",
	//    name, unif_id, value.x, value.y, value.z, _id);
	return (unif_id > 0);
}

bool ShaderProgram::SetUniformArray(const char* name, int count, float* value)
{
	GLint unif_id = GetUniformLocation(name);
	glUniform1fv(unif_id, count, value);

	if(unif_id < 0) {
		Log::GetLog()->Write("shader-program", "Uniform %s not found on shader %d",
			name, _id);
	}


	//Log::GetLog()->Write("Setted uniform %s (id %d) to (%.3f %.3f %.3f) on shader %d",
	//    name, unif_id, value.x, value.y, value.z, _id);
	return (unif_id > 0);
}

bool ShaderProgram::SetUniformArray(const char* name, int count, glm::vec3* value)
{
	GLint unif_id = GetUniformLocation(name);
	glUniform3fv(unif_id, count, (const GLfloat*)&value[0]);

	if(unif_id < 0) {
		Log::GetLog()->Write("shader-program", "Uniform %s not found on shader %d",
			name, _id);
	}


	//Log::GetLog()->Write("Setted uniform %s (id %d) to (%.3f %.3f %.3f) on shader %d",
	//    name, unif_id, value.x, value.y, value.z, _id);
	return (unif_id > 0);
}


GLint ShaderProgram::GetID()
{
    return this->_id;
}

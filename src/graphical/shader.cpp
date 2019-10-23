#include <cstdio>

#include "shader.hpp"
#include "exceptions.hpp"
#include "shader_manager.hpp"
#include "gfx_service.hpp"

using namespace familyline::graphics;

// Open the shader file and compile it
Shader::Shader(const char* file, ShaderType type)
{
	auto content = this->readFile(file);

	GLenum gltype = -1;
	switch (type) {
	case ShaderType::Vertex: gltype = GL_VERTEX_SHADER; break;
	case ShaderType::Fragment: gltype = GL_FRAGMENT_SHADER; break;
	case ShaderType::Geometry: gltype = GL_GEOMETRY_SHADER; break;
	case ShaderType::Compute: gltype = GL_COMPUTE_SHADER; break;
	}

	this->_handle = glCreateShader(gltype);
	const char* cdata = content.c_str();

    glShaderSource(this->_handle, 1, (const char**)&cdata, NULL);

	this->_file = file;
	this->_type = type;

	this->compile();
}


std::string Shader::readFile(const char* file)
{
	std::string data;

	FILE* f = fopen(file, "r");
	if (!f) {
		std::string e = "Error while opening ";
		e.append(file);
		e.append(": ");
		e.append(strerror(errno));
		throw shader_exception(e, errno);
	}

    char s[1024];
    while (!feof(f)) {
        memset(s, 0, 1024);
		fgets(s, 1023, f);

		data.append(s);
	}

	return data;
}

void Shader::compile()
{
	glCompileShader(this->_handle);

	GLint res = GL_FALSE;
	glGetShaderiv(this->_handle, GL_COMPILE_STATUS, &res);

	/* Retrieve shader logs anyway */
	GLint logsize = 0;
	glGetShaderiv(this->_handle, GL_INFO_LOG_LENGTH, &logsize);

	if (res == GL_TRUE && logsize > 1) {
		char* logdata = new char[logsize];

		glGetShaderInfoLog(this->_handle, logsize, NULL, logdata);
		std::string e = "Error while compiling ";
		e.append(this->_file);
		e.append(": ");
		e.append(logdata);
		throw shader_exception(e, -1023);
	}
}

ShaderProgram::ShaderProgram(std::string_view name, std::initializer_list<Shader> shaders)
{
	this->_name = name;
	this->_handle = glCreateProgram();

	for (auto& s : shaders) {
		_files.push_back(std::make_pair(s.getType(), s));
		glAttachShader(_handle, s.getHandle());

	}
}

void ShaderProgram::link()
{
	glLinkProgram(this->_handle);

	GLint res = GL_FALSE;
	glGetProgramiv(this->_handle, GL_LINK_STATUS, &res);

	/* Retrieve shader logs anyway */
	GLint logsize = 0;
	glGetProgramiv(this->_handle, GL_INFO_LOG_LENGTH, &logsize);

	if (logsize > 1) {
		char* logdata = new char[logsize];

		glGetProgramInfoLog(this->_handle, logsize, NULL, logdata);
		std::string e = "Error while compiling shader program '";
		e.append(_name);
		e.append("': ");
		e.append(logdata);

		//if (res == GL_TRUE)
        throw shader_exception(e, 1023);

	}

	GFXService::getShaderManager()->addShader(this);
}

/**
 * Gets the uniform location on cache, or directly from the shader if not there
 *
 * We have a cache because getting this info from the shader is expensive, becayse, well
 * you need to talk to the video card, and even if the video card is fast, the transport is
 * slow, because of the PCI bus.
 * Even if you are using an APU, because APUs use the PCI bus to communicate with the processor
 */
GLint ShaderProgram::getUniformLocation(std::string_view name)
{
    std::string sname{name};
	if (_uniform_cache.find(sname) == _uniform_cache.end()) {
		// Not found. Adds to the cache
        auto uniformVal = glGetUniformLocation(this->_handle, sname.c_str());

        if (uniformVal >= 0)
            _uniform_cache[sname] = uniformVal;

        printf("(%s) %s => %d\n", _name.data(), sname.c_str(),
               uniformVal);
	}

	return _uniform_cache[sname];
}

void ShaderProgram::setUniform(std::string_view name, glm::vec3 val)
{
	glUniform3fv(this->getUniformLocation(name), 1, (const GLfloat*)&val[0]);
}

void ShaderProgram::setUniform(std::string_view name, glm::vec4 val)
{
	glUniform4fv(this->getUniformLocation(name), 1, (const GLfloat*)&val[0]);
}

void ShaderProgram::setUniform(std::string_view name, glm::mat4 val)
{
	glUniformMatrix4fv(this->getUniformLocation(name), 1, GL_FALSE, (const GLfloat*)&val[0][0]);
}

void ShaderProgram::setUniform(std::string_view name, int val)
{
	glUniform1i(this->getUniformLocation(name), val);
}

void ShaderProgram::setUniform(std::string_view name, float val)
{
	glUniform1f(this->getUniformLocation(name), val);
}

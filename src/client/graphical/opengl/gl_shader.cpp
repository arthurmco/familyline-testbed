#include <client/graphical/opengl/gl_shader.hpp>

#ifdef RENDERER_OPENGL

#include <client/graphical/exceptions.hpp>
#include <client/graphical/gfx_service.hpp>
#include <client/graphical/shader_manager.hpp>
#include <cstdio>

#include <regex>
#include <fmt/format.h>
#include <tuple>

#include <common/logger.hpp>

using namespace familyline::graphics;

// Open the shader file and compile it
GLShader::GLShader(const char* file, ShaderType type)
    : Shader(file, type)
{
    auto content = this->readFile(file);

    GLenum gltype = -1;
    const char* strtype;
    switch (type) {
        case ShaderType::Vertex:
            gltype = GL_VERTEX_SHADER;
            strtype = "vertex";
            break;
        case ShaderType::Fragment:
            gltype = GL_FRAGMENT_SHADER;
            strtype = "fragment";
            break;
#ifndef USE_GLES
        /// core OpenGL ES 3.0 does not support geopetry nor compute shaders
    	case ShaderType::Geometry:
            gltype = GL_GEOMETRY_SHADER;
            strtype = "geometry";
            break;
        case ShaderType::Compute:
            gltype = GL_COMPUTE_SHADER;
            strtype = "compute";
            break;
#endif
	default:
            std::string e = "Unsupported shader type for file '";
            e.append(file);
            e.append("' ");
            throw shader_exception(e, -1);
    }

    this->_handle     = glCreateShader(gltype);

    auto& log = LoggerService::getLogger();
    log->write("gl-shader", LogType::Info, "creating shader %s type %s with filesize %zu",
               file, strtype, content.size());

#ifdef USE_GLES
    if (gltype == GL_FRAGMENT_SHADER)
        content = this->addPrecisionData(content);
#endif

    const char* cdata = content.c_str();


    glShaderSource(this->_handle, 1, (const char**)&cdata, NULL);
    
    this->_file = file;
    this->_type = type;

    this->compile();
}

/**
 * Only used in OpenGL ES.
 *
 * Since OpenGL ES requires precision attributes (lowp, mediump, highp),
 * we will add them.
 * By default, this function uses mediump for every vector type, and lowp
 * for floats
 *
 * Returns the shader content, with precision data
 */
std::string GLShader::addPrecisionData(std::string content)
{
    auto& log = LoggerService::getLogger();
    log->write("gl-shader", LogType::Info, "opengl-es: adding precision data for this shader");

    /// Create a source regex and a substitution rule
    /// It will try to match ((<qualifier>) <space> <type> <space> (<name>)), being this
    /// into a variable declaration or a parameter declaration.
    /// The qualifier and the variable name are captured.
    ///
    /// Note that this will not get declarations that are not at the start of a line.
    auto fn_createRegex = [](const char* datatype, const char* precision) {
        std::string restr = fmt::format(R"re(([\n\(,]\s*)([a-z]*\s*){}\s+([a-zA-Z0-9_]*)\s*)re", datatype);
        std::string dst = fmt::format("$1$2{} {} $3", precision, datatype);
        return std::tuple<std::regex, std::string>(std::regex(restr), dst);
    };

    auto alist = std::to_array<std::tuple<std::regex, std::string>>({
            fn_createRegex("vec2", "mediump"),
            fn_createRegex("vec3", "mediump"),
            fn_createRegex("vec4", "mediump"),
            fn_createRegex("float", "lowp"),
        });


    std::string ncontent = content;
    for (auto& [restr, dst] : alist) {
        std::string out = std::regex_replace(ncontent, restr, dst);
        ncontent = out;
    }

    return ncontent;

}


std::regex regInclude(R"(\#include\s*\"(.*)\")");

/**
 * Read and process files
 *
 * Deal with the include files here, too
 */
std::string GLShader::readAndProcessFile(const char* file)
{
    std::string data;

    std::string_view svfile(file);
    std::string_view basePath = svfile.substr(0, svfile.find_last_of('/'));

    FILE* f = fopen(file, "r");
    if (!f) {
        std::string e = fmt::format("Error while opening {}: {}", file, strerror(errno));
        throw shader_exception(e, errno, std::string{file});
    }

    char s[1024];
    while (!feof(f)) {
        memset(s, 0, 1024);
        fgets(s, 1023, f);

        std::string sview(s);

#ifdef USE_GLES
	// In OpenGL ES, we use version 3.0 of the GLSL ES shading
	// language, which has more or less the same features as the
	// GLSL 1.5
	if (sview.find("#version 150") != std::string::npos) {
            data.append("#version 300 es\n");

	    continue;
	}
#endif

        auto words_begin =
            std::sregex_iterator(sview.begin(), sview.end(), regInclude);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            std::string includeFile = match[1].str();

            std::string fullInclude{basePath};
            fullInclude.append("/");
            fullInclude.append(includeFile);

            sview = GLShader::readAndProcessFile(fullInclude.c_str());
            break;
        }

        data.append(sview);
    }

    return data;
}


std::string GLShader::readFile(const char* file)
{
    return this->readAndProcessFile(file);
}

void GLShader::compile()
{
    glCompileShader(this->_handle);

    GLint res = GL_FALSE;
    glGetShaderiv(this->_handle, GL_COMPILE_STATUS, &res);

    /* Retrieve shader logs anyway */
    GLint logsize = 0;
    glGetShaderiv(this->_handle, GL_INFO_LOG_LENGTH, &logsize);

    if (res != GL_TRUE && logsize > 1) {
        char* logdata = new char[logsize];

        glGetShaderInfoLog(this->_handle, logsize, NULL, logdata);
        std::string e = fmt::format("Error while compiling {}: {}", this->_file, logdata);
        throw shader_exception(e, -1023, std::string{_file});
    }
}

GLShaderProgram::GLShaderProgram(std::string_view name, const std::vector<GLShader*>& shaders)
    : ShaderProgram(name)
{
    if (shaders.size() > 0)
        this->_handle = glCreateProgram();

    for (auto s : shaders) {
        _files.push_back(std::make_pair(s->getType(), s));
        glAttachShader(_handle, s->getHandle());
    }
}

void GLShaderProgram::link()
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
        std::string e = fmt::format("Error while compiling shader program '{}': {}", _name, logdata);

        // if (res == GL_TRUE)
        throw shader_exception(e, 1023, std::string{_name});
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
GLint GLShaderProgram::getUniformLocation(std::string_view name)
{
    std::string sname{name};
    if (_uniform_cache.find(sname) == _uniform_cache.end()) {
        // Not found. Adds to the cache
        auto uniformVal = glGetUniformLocation(this->_handle, sname.c_str());

        if (uniformVal >= 0)
            _uniform_cache[sname] = uniformVal;

        return uniformVal;
    }

    return _uniform_cache[sname];
}

void GLShaderProgram::setUniform(std::string_view name, glm::vec3 val)
{
    glUniform3fv(this->getUniformLocation(name), 1, (const GLfloat*)&val[0]);
}

void GLShaderProgram::setUniform(std::string_view name, glm::vec4 val)
{
    glUniform4fv(this->getUniformLocation(name), 1, (const GLfloat*)&val[0]);
}

void GLShaderProgram::setUniform(std::string_view name, glm::mat4 val)
{
    glUniformMatrix4fv(this->getUniformLocation(name), 1, GL_FALSE, (const GLfloat*)&val[0][0]);
}

void GLShaderProgram::setUniform(std::string_view name, int val)
{
    glUniform1i(this->getUniformLocation(name), val);
}

void GLShaderProgram::setUniform(std::string_view name, float val)
{
    glUniform1f(this->getUniformLocation(name), val);
}

void GLShaderProgram::use()
{
    glUseProgram(this->_handle);
}

#endif

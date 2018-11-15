#include "Shader.hpp"

using namespace familyline::graphics;

Shader::Shader(const char* path, int type)
{
    /*  Retrieve the file size before opening it, so we can discover some
        errors there (like 'file does not exist' )*/
    struct stat st;
    if (stat(path, &st) < 0) {
        int err = errno;
        if (err == ENOENT)
            throw shader_exception("File does not exist",
                errno, path, type);
        else
            throw shader_exception("Could not stat shader file",
                errno, path, type);
    }


    size_t fsize = st.st_size;

    FILE* fShader = fopen(path, "r");

    if (!fShader)
    {
        throw shader_exception("Could not open shader file", errno, path, type);
    }

    /* Read the file data, using the RightWay(TM) method */
    char* filedata = new char[fsize+1];
    memset((void*)filedata, 0, fsize+1);

    char* fileptr = filedata;

    while (!feof(fShader)) {
        fgets(fileptr, fsize, fShader);
        fileptr += strlen(fileptr);
    }

    filedata[fsize] = 0;

    fclose(fShader);

    /* Create the shader */
    GLenum stype = 0;

    switch (type) {
    case SHADER_PIXEL:      stype = GL_FRAGMENT_SHADER; break;
    case SHADER_VERTEX:     stype = GL_VERTEX_SHADER;   break;
    case SHADER_GEOMETRY:   stype = GL_GEOMETRY_SHADER; break;
    default:
        throw shader_exception("Invalid shader type", errno, path, type);
    }

    this->_id = glCreateShader(stype);
    glShaderSource(this->_id, 1, (const GLchar**)&filedata, NULL);

    Log::GetLog()->Write("shader", "Shader %s sucessfully compiled as type %d",
        path, type);

    this->_path = std::string{path};
    this->_type = type;

    delete[] filedata;
}

bool Shader::Compile()
{
    glCompileShader(this->_id);

    GLint res = GL_FALSE;
    glGetShaderiv(this->_id, GL_COMPILE_STATUS, &res);

    /* Retrieve shader logs anyway */
    GLint logsize = 0;
    glGetShaderiv(this->_id, GL_INFO_LOG_LENGTH, &logsize);

    if (logsize > 1) {
        char* logdata = new char[logsize];

        glGetShaderInfoLog(this->_id, logsize, NULL, logdata);
        Log::GetLog()->Warning("shader", "Shader %s retrieved a log: \n%s",
            this->_path.c_str(), logdata);
		throw shader_exception(logdata, 0, this->_path.c_str(), this->_type);

		delete[] logdata;
    }

    return (res == GL_TRUE);
}

GLint Shader::GetID() const
{
    return this->_id;
}
const char* Shader::GetPath() const
{
    return this->_path.c_str();
}
int Shader::GetType() const
{
    return this->_type;
}

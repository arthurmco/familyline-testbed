#include "GFXExceptions.hpp"

using namespace Tribalia::Graphics;

renderer_exception::renderer_exception(std::string msg, int code)
    : std::runtime_error(msg)
    {
        this->code = code;
    }

shader_exception::shader_exception(std::string msg, int code,
        const char* file, int type)
        : std::runtime_error(msg)
        {
            this->code = code;
            this->file = std::string{file};
            this->type = type;
        }

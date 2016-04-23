#include "GFXExceptions.hpp"

using namespace Tribalia::Graphics;

renderer_exception::renderer_exception(std::string msg, int code)
    : std::runtime_error(msg)
    {
        this->code = code;
    }

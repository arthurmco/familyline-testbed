/***
    Class that contains all graphical exceptions

    Copyright (C) 2016 Arthur M

***/

#include <stdexcept>

#ifndef GFXEXCEPTIONS_HPP
#define GFXEXCEPTIONS_HPP

namespace Tribalia {
namespace Graphics {

    class renderer_exception : public std::runtime_error
    {
    public:
        explicit renderer_exception(std::string msg, int code);
        int code;
    };

    class shader_exception : public std::runtime_error
    {
    public:
        explicit shader_exception(std::string msg, int code,
            const char* file, int type);
        int code;
        std::string file;
        int type;
    };

    class mesh_exception : public std::runtime_error
    {
    public:
        explicit mesh_exception(std::string msg, int code,
            const char* file);
        int code;
        std::string file;
    };


} /* Graphics */
} /* Tribalia */



#endif /* end of include guard: GFXEXCEPTIONS_HPP */

/***
    Class that contains all graphical exceptions

    Copyright (C) 2016 Arthur M
***/

#include <stdexcept>

#ifndef GFXEXCEPTIONS_HPP
#define GFXEXCEPTIONS_HPP

namespace familyline::graphics {

    class window_exception : public std::runtime_error
    {
    public:
        explicit window_exception(std::string msg, int code);

        int code;
    };

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

    class material_exception : public std::runtime_error
    {
    public:
        explicit material_exception(std::string msg, int code,
            const char* file);
        int code;
        std::string file;
    };

    class asset_exception : public std::runtime_error
    {
    public:
        explicit asset_exception(void* assetptr, const char* msg);
        void* assetptr;
    };

} /* familyline::graphics */



#endif /* end of include guard: GFXEXCEPTIONS_HPP */

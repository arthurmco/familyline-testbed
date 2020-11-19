#pragma once

#include <client/graphical/opengl/gl_headers.hpp>
#include <string_view>
#include <initializer_list>
#include <functional>

#ifdef USE_GLES

/**
 * OpenGL ES convenience functions.
 *
 * Mostly implementations of extensions we will use, and a function to
 * query them
 */

namespace familyline::graphics {

    bool isFunctionPresent(std::string_view name);
    void* getFunctionPointer(std::string_view name);
    
    bool isExtensionPresent(std::string_view name);    
    bool isExtensionListPresent(std::initializer_list<std::string_view> names);

    
    /**
     * Call a OpenGL ES function from an extension
     *
     * Since GLEw is not compatible with OpenGL ES, we have to make something
     * similar.
     *
     * If you specify the correct function pointer type, it will check
     * the types of the arguments correctly.
     */
    template <typename FPtr, typename ...Ts>
    void callGLFunction(std::string_view name, Ts... args)
    {
        auto f = std::bind((FPtr) getFunctionPointer(name), args...);
        f();
    }
}


#endif

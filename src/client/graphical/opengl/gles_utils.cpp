#include <client/graphical/opengl/gles_utils.hpp>

using namespace familyline::graphics;

#ifdef USE_GLES

bool familyline::graphics::isExtensionPresent(std::string_view name)
{
    GLint n, i;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);

    for (i = 0; i < n; i++) {
        std::string_view ext{ (const char*) glGetStringi(GL_EXTENSIONS, i) };
        if (name == ext)
            return true;
    }

    return false;
}

void* familyline::graphics::getFunctionPointer(std::string_view name)
{
    return SDL_GL_GetProcAddress(name.data());
}


bool familyline::graphics::isFunctionPresent(std::string_view name)
{
    return getFunctionPointer(name) != nullptr;
}


bool familyline::graphics::isExtensionListPresent(std::initializer_list<std::string_view> names)
{
    bool res = false;
    for (auto n : names) {
        res = res && isExtensionPresent(n);
    }

    return true;
}

#endif

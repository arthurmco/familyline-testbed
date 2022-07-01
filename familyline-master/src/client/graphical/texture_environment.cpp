#include <client/graphical/texture_environment.hpp>

using namespace familyline::graphics;

/**
 * Create an unique ptr of a SDL_Surface
 *
 * Useful so we do not need to worry about managing it later
 */
surface_unique_ptr familyline::graphics::make_surface_unique_ptr(SDL_Surface *s)
{
    return std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>(s, &SDL_FreeSurface);
}


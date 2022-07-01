#pragma once

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <tl/expected.hpp>

namespace familyline::graphics
{
using surface_unique_ptr = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>;

using TextureHandle = uint64_t;

/**
 * Information about some texture
 */
struct Texture {
    /// Texture data, in a common format, just a SDL surface, so that the
    /// client will be able to reload it in case of it being removed
    /// from the GPU.
    surface_unique_ptr data;

    /// Texture handle, given by the graphical API
    /// If this is nullopt, this means that this texture has no handle, and
    /// probably is not uploaded into the videocard
    std::optional<uintptr_t> renderer_handle;

    /// If the texture is active, what texture unit it is bound to
    std::optional<uint32_t> texture_unit;

    /// A reference count, to check if the texture is used multiple times, and to
    /// avoid uploading multiple textures.
    int refcount = 0;

    Texture(surface_unique_ptr data)
        : data(std::move(data)),
          renderer_handle(std::nullopt),
          texture_unit(std::nullopt),
          refcount(0)
    {
    }
};

/**
 * Create an unique ptr of a SDL_Surface
 *
 * Useful so we do not need to worry about managing it later
 */
surface_unique_ptr make_surface_unique_ptr(SDL_Surface *s);

}  // namespace familyline::graphics

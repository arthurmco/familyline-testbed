#pragma once

/**
 * Texture environment for opengl devices
 *
 * Copyright (C) 2021 Arthur Mendes
 */

#include <client/graphical/opengl/gl_headers.hpp>

#ifdef RENDERER_OPENGL

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <client/graphical/texture_environment.hpp>

namespace familyline::graphics {

class GLTextureEnvironment : public familyline::graphics::TextureEnvironment
{
public:
    /**
     * Initialize the environment, Return True on success, False on fail
     */
    virtual bool initialize();

    virtual tl::expected<
        std::unique_ptr<familyline::graphics::Texture>, familyline::graphics::ImageError>
        loadTextureFromFile(std::string_view);

    /**
     * Load a texture from a memory buffer
     *
     * Alongside the memory buffer, you need to specify the width, height and
     * texture color format
     */
    virtual tl::expected<
        std::unique_ptr<familyline::graphics::Texture>, familyline::graphics::ImageError>
    loadTextureFromMemory(
        std::span<uint8_t> data, size_t width, size_t height,
        familyline::graphics::TextureFormat format);
   
    /**
     * Gets the maximum texture size you can send to the GPU
     *
     * If the size is 0x0, means that something happened
     */
    virtual std::tuple<unsigned, unsigned> getTextureMaxSize() const;

    /**
     * Upload a texture to the videocard
     *
     * Returns the API-specific texture code on success, and an error on failure.
     */
    virtual tl::expected<uintptr_t, familyline::graphics::TextureError> uploadTexture(
        familyline::graphics::Texture &);

    /**
     * Set the texture contents
     *
     * By contents, you can set the texture size, format and data.
     * But unlike the Theseus ship, if you set every detail of a texture, it
     * still stays the same, because the handle is the same.
     *
     * Returns the API-specific texture code on success, or an error on failure.
     */
    virtual tl::expected<uintptr_t, familyline::graphics::ImageError> setTextureData(
        familyline::graphics::Texture &, std::span<uint8_t> data, size_t width, size_t height,
        familyline::graphics::TextureFormat format);

    /**
     * Unload a texture from the videocard
     *
     * Returns the (now not valid) API-specific texture code on success, or an
     * error on failure
     */
    virtual tl::expected<uintptr_t, familyline::graphics::TextureError> unloadTexture(
        familyline::graphics::Texture &);

    /**
     * Set a certain texture to be the current one in the specified texture unit.
     *
     * This will affect rendering. For example, if your shader needs a texture at
     * texture unit 0, the texture you set to the current one is the one that
     * will be rendered
     */
    virtual tl::expected<uintptr_t, familyline::graphics::TextureError> bindTexture(
        familyline::graphics::Texture &, unsigned unit);

    /**
     * Unbind a texture from a specified texture unit
     */
    virtual tl::expected<uintptr_t, familyline::graphics::TextureError> unbindTexture(
        unsigned unit);

    virtual ~GLTextureEnvironment();

private:
    bool started_ = false;

    int current_unit_ = 0;
    std::array<GLuint /* texture */, 32 /* unit */> current_bound_ = {};

    /**
     * Update byte data of the current bound texture with the contents of
     * the passed SDL_Surface 
     */
    void updateBoundTextureData(SDL_Surface& data);
    
};
}  // namespace familyline::graphics

#endif

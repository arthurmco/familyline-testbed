#pragma once

/*
  Represents a texture environment

  A superclass for all the specific texture features from a graphical API.

  Copyright (C) 2021 Arthur Mendes
*/

#include <client/graphical/texture.hpp>

#include <array>
#include <cstdint>
#include <memory>
#include <optional>

#include <span>

#include <tl/expected.hpp>

namespace familyline::graphics
{

/**
 * An error that might happen if the image could not be loaded
 */
enum class ImageError {
    ImageFormatUnknown,
    ImageNotExists,

    /// An error happened while creating a texture from a texture atlas
    TextureAtlasError,
    
    UnknownError,
};

/**
 * An error that might happen if we have a problem with textures
 * (being on upload or on removal)
 */
enum class TextureError {
    /// Insufficient memory to load the texture in the videocard
    InsufficientMemory,

    /// The texture you passed was not found
    TextureNotFound,

    /// Tried to upload a texture that was already been uploaded
    /// If this error is propagated, is more fault of the texture manager than
    /// of anything else.
    TextureAlreadyThere,

    /// The specified texture unit is invalid
    /// The real cause is because you probably used all valid texture units.
    TextureUnitInvalid,

    /// You are removing a texture that is currently bound.
    TextureIsBound,

    /// The texture format isnt supported and could not be converted
    /// to a supported format.
    BadTextureFormat,

    UnknownError
};

/// Texture format
///
/// Basically, the order of the pixels in a image, and their format.
enum class TextureFormat {
    ARGB32,
    ABGR32,
    RGBA32,
};

/**
 * An abstraction of any API routines for texture creation and deletion
 */
class TextureEnvironment
{
public:
    /**
     * Initialize the environment, Return True on success, False on fail
     */
    virtual bool initialize() = 0;

    /**
     * Load a texture from a file
     */
    virtual tl::expected<std::unique_ptr<Texture>, ImageError> loadTextureFromFile(
        std::string_view) = 0;

    /**
     * Load a texture from a memory buffer
     *
     * Alongside the memory buffer, you need to specify the width, height and
     * texture color format
     */
    virtual tl::expected<std::unique_ptr<Texture>, ImageError> loadTextureFromMemory(
        std::span<uint8_t> data, size_t width, size_t height, TextureFormat format) = 0;

    /**
     * Gets the maximum texture size you can send to the GPU
     *
     * If the size is 0x0, means that something happened
     */
    virtual std::tuple<unsigned, unsigned> getTextureMaxSize() const = 0;

    /**
     * Upload a texture to the videocard
     *
     * Returns the API-specific texture code on success, or an error on failure.
     */
    virtual tl::expected<uintptr_t, TextureError> uploadTexture(Texture &) = 0;

    /**
     * Set the texture contents
     *
     * By contents, you can set the texture size, format and data.
     * But unlike the Theseus ship, if you set every detail of a texture, it
     * still stays the same, because the handle is the same.
     *
     * Returns the API-specific texture code on success, or an error on failure.
     */
    virtual tl::expected<uintptr_t, ImageError> setTextureData(
        Texture &, std::span<uint8_t> data, size_t width, size_t height, TextureFormat format) = 0;

    /**
     * Unload a texture from the videocard
     *
     * Returns the (now not valid) API-specific texture code on success, or an
     * error on failure
     */
    virtual tl::expected<uintptr_t, TextureError> unloadTexture(Texture &) = 0;

    /**
     * Set a certain texture to be the current one in the specified texture unit.
     *
     * This will affect rendering. For example, if your shader needs a texture at
     * texture unit 0, the texture you set to the current one is the one that
     * will be rendered
     */
    virtual tl::expected<uintptr_t, TextureError> bindTexture(Texture &, unsigned unit) = 0;

    /**
     * Unbind a texture from a specified texture unit
     *
     * Returns the renderer-specific texture handle of the texture that was
     * bound to that unit, or 0 if no texture was bound there.
     */
    virtual tl::expected<uintptr_t, TextureError> unbindTexture(unsigned unit) = 0;

    virtual ~TextureEnvironment() {}
};


}  // namespace familyline::graphics



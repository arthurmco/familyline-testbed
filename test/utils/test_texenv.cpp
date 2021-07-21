#include "test_texenv.hpp"

using namespace familyline::graphics;

bool TestTextureEnvironment::initialize()
{
    int flags     = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    int initflags = IMG_Init(flags);

    if ((initflags & flags) != flags) {
        printf("Not all texture formats were supported (%08x != %08x)\n", flags, initflags);
        return false;
    }

    started_ = true;
    return true;
}

/**
 * Gets the maximum texture size you can send to the GPU
 *
 * If the size is 0x0, means that something happened
 */
std::tuple<unsigned, unsigned> TestTextureEnvironment::getTextureMaxSize() const
{
    return std::make_tuple(2048, 2048);
}

tl::expected<std::unique_ptr<Texture>, ImageError> TestTextureEnvironment::loadTextureFromFile(
    std::string_view file)
{
    SDL_Surface *surface = IMG_Load(file.data());
    if (!surface) {
        printf("%s failed to load. Why? I do not know.\n", file.data());
        printf("Remember to use tl::expected here when porting to the real deal\n");

        std::string err = IMG_GetError();
        printf("Error: %s\n", err.c_str());

        if (err.find("Couldn't open") != std::string::npos) {
            return tl::make_unexpected(ImageError::ImageNotExists);
        } else {
            return tl::make_unexpected(ImageError::UnknownError);
        }
    }

    auto data = make_surface_unique_ptr(surface);
    return std::make_unique<Texture>(std::move(data));
}

/**
 * Load a texture from a memory buffer
 *
 * Alongside the memory buffer, you need to specify the width, height and
 * texture color format
 */
tl::expected<std::unique_ptr<Texture>, ImageError> TestTextureEnvironment::loadTextureFromMemory(
    std::span<uint8_t> data, size_t width, size_t height, TextureFormat format)
{
    int depth      = 0;
    int pitch      = width;
    uint32_t rmask = 0xff000000;
    uint32_t gmask = 0x00ff0000;
    uint32_t bmask = 0x0000ff00;
    uint32_t amask = 0x000000ff;

    switch (format) {
        case TextureFormat::ARGB32:
            depth = 32;
            pitch = width * (depth / 4);
            rmask = 0x0000ff00;
            gmask = 0x00ff0000;
            bmask = 0xff000000;
            amask = 0x000000ff;
            break;
        case TextureFormat::ABGR32:
            depth = 32;
            pitch = width * (depth / 4);
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
            break;
        case TextureFormat::RGBA32:
            depth = 32;
            pitch = width * (depth / 4);
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
            break;
    }

    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
        data.data(), width, height, depth, pitch, rmask, gmask, bmask, amask);
    if (!surface) {
        printf("failed to create image!\n");
        printf("error: %s\n", SDL_GetError());
        return tl::make_unexpected(ImageError::UnknownError);
    }

    auto surfacedata = make_surface_unique_ptr(surface);
    return std::make_unique<Texture>(std::move(surfacedata));
}

/**
 * Upload a texture to the videocard
 *
 * Returns the API-specific texture code on success, and an error on failure.
 */
tl::expected<uintptr_t, TextureError> TestTextureEnvironment::uploadTexture(Texture &t)
{
    static uint32_t new_handle = 1;

    if (t.renderer_handle) {
        tl::make_unexpected(TextureError::TextureAlreadyThere);
    }

    constexpr size_t MAX_TEXTURES = 16;

    if (textures_.size() >= MAX_TEXTURES) {
        tl::make_unexpected(TextureError::InsufficientMemory);
    }

    auto renderer_handle = new_handle++;
    t.renderer_handle    = std::make_optional((uintptr_t)renderer_handle);

    textures_[renderer_handle] = t.data.get();
    printf(
        "\t added texture handle %08x (%zu / %zu)\n", renderer_handle, textures_.size(),
        MAX_TEXTURES);

    return renderer_handle;
}

/**
 * Set the texture contents
 *
 * By contents, you can set the texture size, format and data.
 * But unlike the Theseus ship, if you set every detail of a texture, it
 * still stays the same, because the handle is the same.
 *
 * Returns the API-specific texture code on success, or an error on failure.
 */
tl::expected<uintptr_t, ImageError> TestTextureEnvironment::setTextureData(
    Texture &t, std::span<uint8_t> data, size_t width, size_t height, TextureFormat format)
{
    return this->loadTextureFromMemory(data, width, height, format)
        .map([&](auto ntexture) {
            t.data = std::move(ntexture->data);
            return uintptr_t(t.renderer_handle ? *t.renderer_handle : 0);
        })
        .map_error([](auto error) { return error; });
}

/**
 * Unload a texture from the videocard
 *
 * Returns the (now not valid) API-specific texture code on success, or an
 * error on failure
 */
tl::expected<uintptr_t, TextureError> TestTextureEnvironment::unloadTexture(Texture &t)
{
    if (!t.renderer_handle) return tl::make_unexpected(TextureError::TextureNotFound);

    textures_.erase(*t.renderer_handle);
    return *t.renderer_handle;
}

/**
 * Set a certain texture to be the current one in the specified texture unit.
 *
 * This will affect rendering. For example, if your shader needs a texture at
 * texture unit 0, the texture you set to the current one is the one that
 * will be rendered
 */
tl::expected<uintptr_t, TextureError> TestTextureEnvironment::bindTexture(Texture &t, unsigned unit)
{
    if (unit >= current_textures_.size())
        return tl::make_unexpected(TextureError::TextureUnitInvalid);

    if (!t.renderer_handle) return tl::make_unexpected(TextureError::TextureNotFound);

    current_textures_[unit] = *t.renderer_handle;
    return *t.renderer_handle;
}

/**
 * Unbind a texture from a specified texture unit
 */
tl::expected<uintptr_t, TextureError> TestTextureEnvironment::unbindTexture(unsigned unit)
{
    if (unit >= current_textures_.size())
        return tl::make_unexpected(TextureError::TextureUnitInvalid);

    auto handle             = current_textures_[unit];
    current_textures_[unit] = 0;

    return handle;
}

TestTextureEnvironment::~TestTextureEnvironment()
{
    if (started_) {
        IMG_Quit();
    }
}


#include <client/graphical/opengl/gl_texture_environment.hpp>

#ifdef RENDERER_OPENGL

#include <common/logger.hpp>

using namespace familyline::graphics;

bool GLTextureEnvironment::initialize()
{
    auto &log     = LoggerService::getLogger();
    int flags     = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
    int initflags = IMG_Init(flags);

    if ((initflags & flags) != flags) {
        log->write(
            "gl-texture-env", LogType::Error,
            "Not all texture formats were supported (%08x != %08x)\n", flags, initflags);
        return false;
    }

    log->write("gl-texture-env", LogType::Info, "opengl texture environment initialized");

    started_ = true;
    return true;
}

/**
 * Gets the maximum texture size you can send to the GPU
 *
 * If the size is 0x0, means that something happened
 */
std::tuple<unsigned, unsigned> GLTextureEnvironment::getTextureMaxSize() const
{
    GLint texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texture_size);

    if (texture_size > 0)
        return std::make_tuple(texture_size, texture_size);
    else
        return std::make_tuple(0, 0);
}

tl::expected<std::unique_ptr<Texture>, ImageError> GLTextureEnvironment::loadTextureFromFile(
    std::string_view file)
{
    auto &log = LoggerService::getLogger();

    SDL_Surface *surface = IMG_Load(file.data());
    if (!surface) {
        log->write("gl-texture-env", LogType::Error, "%s failed to load", file.data());

        std::string err = IMG_GetError();
        log->write("gl-texture-env", LogType::Error, "Error: %s\n", err.c_str());

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
tl::expected<std::unique_ptr<Texture>, ImageError> GLTextureEnvironment::loadTextureFromMemory(
    std::span<uint8_t> data, size_t width, size_t height, TextureFormat format)
{
    auto &log = LoggerService::getLogger();

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
        log->write("gl-texture-env", LogType::Error, "failed to create image!\n");
        log->write("gl-texture-env", LogType::Error, "error: %s\n", SDL_GetError());
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
tl::expected<uintptr_t, TextureError> GLTextureEnvironment::uploadTexture(Texture &t)
{
    auto &log = LoggerService::getLogger();

    if (t.renderer_handle) {
        return tl::make_unexpected(TextureError::TextureAlreadyThere);
    }

    auto currentbind = current_bound_[current_unit_];

    GLuint tex_handle = 0;
    glGenTextures(1, &tex_handle);
    glBindTexture(GL_TEXTURE_2D, tex_handle);

    glGetError();
    GLenum glerr = GL_NO_ERROR;
    if ((glerr = glGetError()) != GL_NO_ERROR) {
        log->write(
            "gl-texture-env", LogType::Error, "GL error %#x while binding texture handle %d", glerr,
            tex_handle);
        return tl::make_unexpected(TextureError::UnknownError);
    }

    t.renderer_handle = std::make_optional((uintptr_t)tex_handle);
    SDL_Surface *data = t.data.get();

    GLenum format = GL_RGB;
    switch (data->format->format) {
        case SDL_PIXELFORMAT_RGBA32: format = GL_RGBA; break;
        case SDL_PIXELFORMAT_BGRA32: format = GL_BGRA; break;
        default: format = GL_RGB; break;
    }

    GLenum dest_format = GL_SRGB;
    if (format == GL_RGBA || format == GL_BGRA) {
        dest_format = GL_SRGB_ALPHA;
    }

    SDL_LockSurface(data);
    glTexImage2D(
        GL_TEXTURE_2D, 0, dest_format, data->w, data->h, 0, format, GL_UNSIGNED_BYTE, data->pixels);
    SDL_UnlockSurface(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    log->write("gl-texture-env", LogType::Info, "\t added texture handle %08x", tex_handle);

    if (currentbind != 0) {
        glBindTexture(GL_TEXTURE_2D, currentbind);
    }

    return tex_handle;
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
tl::expected<uintptr_t, ImageError> GLTextureEnvironment::setTextureData(
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
tl::expected<uintptr_t, TextureError> GLTextureEnvironment::unloadTexture(Texture &t)
{
    if (!t.renderer_handle) return tl::make_unexpected(TextureError::TextureNotFound);

    GLuint handle = *t.renderer_handle;
    glDeleteTextures(1, &handle);

    return handle;
}

/**
 * Set a certain texture to be the current one in the specified texture unit.
 *
 * This will affect rendering. For example, if your shader needs a texture at
 * texture unit 0, the texture you set to the current one is the one that
 * will be rendered
 */
tl::expected<uintptr_t, TextureError> GLTextureEnvironment::bindTexture(Texture &t, unsigned unit)
{
    if (unit >= current_bound_.size())
        return tl::make_unexpected(TextureError::TextureUnitInvalid);

    if (!t.renderer_handle) return tl::make_unexpected(TextureError::TextureNotFound);

    GLuint handle        = *t.renderer_handle;
    int current_unit_    = unit;
    current_bound_[unit] = handle;

    glBindTexture(GL_TEXTURE_2D, handle);

    return handle;
}

/**
 * Unbind a texture from a specified texture unit
 */
tl::expected<uintptr_t, TextureError> GLTextureEnvironment::unbindTexture(unsigned unit)
{
    if (unit >= current_bound_.size())
        return tl::make_unexpected(TextureError::TextureUnitInvalid);

    int current_unit_    = unit;
    auto handle = current_bound_[unit];
    current_bound_[unit] = 0;

    glBindTexture(GL_TEXTURE_2D, 0);

    return handle;
}

GLTextureEnvironment::~GLTextureEnvironment()
{
    if (started_) {
        IMG_Quit();
    }
}

#endif

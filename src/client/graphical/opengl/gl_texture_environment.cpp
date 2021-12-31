#include <client/graphical/opengl/gl_texture_environment.hpp>

#ifdef RENDERER_OPENGL

#include <fmt/format.h>

#include <common/logger.hpp>

using namespace familyline::graphics;

/**
 * Reference count to control how much texture environments are initalized
 *
 * Since IMG_Init() and IMG_Quit() are global, we must control them, so if we initialize two
 * environments, they do not initialize twice, or deinitialize each other
 */
static int initref = 0;

bool GLTextureEnvironment::initialize()
{
    auto &log = LoggerService::getLogger();
    int flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;

    int initflags = IMG_Init(flags);

    if ((initflags & flags) != flags) {
        log->write(
            "gl-texture-env", LogType::Error,
            "Not all texture formats were supported ({:08x} != {:08x})\n", flags, initflags);
        return false;
    }

    initref++;
    log->write(
        "gl-texture-env", LogType::Info, "opengl texture environment initialized (envcount {})",
        initref);

    #ifdef USE_GLES
    log->write(
        "gl-texture-env", LogType::Info, "running under OpenGL ES. Texture options might be limited",
        initref);

    #endif

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
        log->write("gl-texture-env", LogType::Error, "{} failed to load", file.data());

        std::string err = IMG_GetError();
        log->write("gl-texture-env", LogType::Error, "Error: {}\n", err.c_str());

        if (err.find("Couldn't open") != std::string::npos) {
            return tl::make_unexpected(ImageError::ImageNotExists);
        } else {
            return tl::make_unexpected(ImageError::UnknownError);
        }
    }

    log->write("gl-texture-env", LogType::Info, "loaded texture '{}'", file.data());
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
        log->write("gl-texture-env", LogType::Error, "error: {}\n", SDL_GetError());
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
            "gl-texture-env", LogType::Error, "GL error 0x{:x} while binding texture handle {}", glerr,
            tex_handle);
        return tl::make_unexpected(TextureError::UnknownError);
    }

    t.renderer_handle = std::make_optional((uintptr_t)tex_handle);
    SDL_Surface *data = t.data.get();

    glGetError();
    this->updateBoundTextureData(*data);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        switch (err) {
        case GL_INVALID_OPERATION:
            log->write("gl-texture-env", LogType::Info, "setting texture handle {:08x} data returned a GL_INVALID_OPERATION. The texture format might be unsupported  ", tex_handle);
            return tl::make_unexpected(TextureError::BadTextureFormat);

        }
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    log->write("gl-texture-env", LogType::Info, "added texture handle {:08x}", tex_handle);

    if (currentbind != 0) {
        glBindTexture(GL_TEXTURE_2D, currentbind);
    }

    return tex_handle;
}

/**
 * Update byte data of the current bound texture with the contents of
 * the passed SDL_Surface
 *
 * The surface is not a const reference because we lock it, and locking it might change
 * some data structures there.
 */
void GLTextureEnvironment::updateBoundTextureData(SDL_Surface &data)
{
    auto &log             = LoggerService::getLogger();
    std::string formatstr = "";
    GLenum format         = GL_RGB;

    SDL_Surface* converted_surf = nullptr;

    switch (data.format->format) {
        case SDL_PIXELFORMAT_RGBA32:
            formatstr = "RGBA32";
            format    = GL_RGBA;
            break;
        case SDL_PIXELFORMAT_BGRA32:
            formatstr = "BGRA32";
            format    = GL_BGRA;
            break;
        case SDL_PIXELFORMAT_RGB24:
            formatstr = "RGB24";
            format    = GL_RGB;
            break;
#ifdef USE_GLES
        case SDL_PIXELFORMAT_BGR24:
            log->write(
                "gl-texture-env", LogType::Warning, "format BGR24 not supported, converting to RGB24",
                data.w, data.h, formatstr.c_str());

            converted_surf = SDL_ConvertSurfaceFormat(&data, SDL_PIXELFORMAT_RGB24, 0);
            if (!converted_surf) {
                log->write(
                    "gl-texture-env", LogType::Error, "conversion failed: {}", SDL_GetError());
            }

            formatstr = "BGR24 -> RGB24";
            format    = GL_RGB;
            break;
#else
        case SDL_PIXELFORMAT_BGR24:
            formatstr = "BGR24";
            format    = GL_BGR;
            break;
#endif
        default:
            formatstr = fmt::format("unknown ({:04x})", data.format->format);
            format    = GL_RGB;
            break;
    }

#ifdef USE_GLES
    GLenum dest_format = GL_RGB8;
    if (format == GL_RGBA || format == GL_BGRA) {
        dest_format = GL_RGBA8;
    }
#else
    GLenum dest_format = GL_SRGB;
    if (format == GL_RGBA || format == GL_BGRA) {
        dest_format = GL_SRGB_ALPHA;
    }
#endif
    log->write(
        "gl-texture-env", LogType::Info, "setting texture data: width={:4d}, height={:4d}, format={}",
        data.w, data.h, formatstr.c_str());

    SDL_LockSurface(&data);
    if (!converted_surf) {
        glTexImage2D(
            GL_TEXTURE_2D, 0, dest_format, data.w, data.h, 0, format, GL_UNSIGNED_BYTE, data.pixels);
    } else {
        glTexImage2D(
            GL_TEXTURE_2D, 0, dest_format, data.w, data.h, 0, format, GL_UNSIGNED_BYTE,
            converted_surf->pixels);
        SDL_FreeSurface(converted_surf);
    }

    SDL_UnlockSurface(&data);

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
            auto &log = LoggerService::getLogger();
            log->write(
                "gl-texture-env", LogType::Info, "setting data of texture with handle 0x{:08x}",
                *t.renderer_handle);

            t.data = std::move(ntexture->data);

            auto currentbind = current_bound_[current_unit_];
            auto handle = *t.renderer_handle;

            glBindTexture(GL_TEXTURE_2D, handle);
            this->updateBoundTextureData(*t.data.get());
            if (currentbind != 0) {
                glBindTexture(GL_TEXTURE_2D, currentbind);
            }

            return handle;
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
    auto &log = LoggerService::getLogger();

    if (!t.renderer_handle) {
        log->write(
            "gl-texture-env", LogType::Error, "Texture not loaded into the videocard");
        return tl::make_unexpected(TextureError::TextureNotFound);
    }

    if (unit >= current_bound_.size()) {
        log->write(
            "gl-texture-env", LogType::Error, "Texture unit is out of the bounds for texture {:x}",
            *t.renderer_handle);
        return tl::make_unexpected(TextureError::TextureUnitInvalid);
    }

    GLuint handle        = *t.renderer_handle;
    int current_unit_    = unit;
    current_bound_[unit] = handle;

    glBindTexture(GL_TEXTURE_2D, handle);
#if 0
    log->write("gl-texture-env", LogType::Debug, "opengl texture 0x{:x} bound to unit {}", handle, unit);
#endif

    return handle;
}

/**
 * Unbind a texture from a specified texture unit
 */
tl::expected<uintptr_t, TextureError> GLTextureEnvironment::unbindTexture(unsigned unit)
{
    auto &log = LoggerService::getLogger();
    if (unit >= current_bound_.size()) return tl::make_unexpected(TextureError::TextureUnitInvalid);

    int current_unit_    = unit;
    auto handle          = current_bound_[unit];
    current_bound_[unit] = 0;

    glBindTexture(GL_TEXTURE_2D, 0);
#if 0
    log->write("gl-texture-env", LogType::Debug, "opengl texture 0x{:x} unbound from unit {}", handle, unit);
#endif
    return handle;
}

GLTextureEnvironment::~GLTextureEnvironment()
{
    initref--;
    if (started_ && initref == 0) {
        IMG_Quit();
    }
}

#endif

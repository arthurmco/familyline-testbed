#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_surface.h>
#include <fmt/format.h>

#include <client/graphical/texture_environment.hpp>
#include <client/graphical/texture_manager.hpp>
#include <common/logger.hpp>
#include <tl/expected.hpp>

using namespace familyline::graphics;

/**
 * Hash a filename, to be part of the texture key for our map, and also
 * as a part of the texture handle you will pass to the key.
 */
uint64_t TextureManager::hashFilename(std::string_view filename)
{
    // A very idiotic hash algorithm, whose base number is based on a brazilian
    // meme Please replace this with CRC.
    uint64_t base = 0x30696f6b616c616d;

    uint64_t idx = 0;
    for (auto i = 0; i < 4; i++) {
        for (auto &fchar : filename) {
            uint64_t fchar64 = (uint64_t)fchar;
            base |= (fchar64 * (idx + 1));
            base = std::rotr(base, 8 + i);
            base += (fchar64 * (idx << 32));
            base = std::rotr(base, 8 + i);
            base ^= ((fchar64 | 0x3000) * 0xff00ff00);
            base = std::rotr(base, 8 + i);

            idx++;
        }
    }

    base *= idx;
    base = std::rotr(base, 32);

    return base;
}

tl::expected<TextureHandle, ImageError> TextureManager::addTexture(
    std::string_view filename, std::unique_ptr<Texture> texture)
{
    auto handle = hashFilename(filename);
    auto &log   = LoggerService::getLogger();

    if (auto texiter = textures_.find(handle); texiter != textures_.end()) {
        log->write("texture-manager", LogType::Warning, "texture '{}' already added", filename);
        texiter->second->refcount++;
        return tl::expected<TextureHandle, ImageError>(handle);
    } else {
        textures_[handle] = std::move(texture);
    }

    return tl::expected<TextureHandle, ImageError>(handle);
}

/**
 * Load a texture.
 *
 * Returns a texture handle, basically a hash of the texture information, so
 * we can rapidly detect if we loaded the texture twice
 */
tl::expected<TextureHandle, ImageError> TextureManager::loadTexture(std::string_view filename)
{
    return environ_->loadTextureFromFile(filename).and_then([this, &filename](auto &&texture) {
        return this->addTexture(filename, std::move(texture));
    });
}

/**
 * Load multiple textures from a texture atlas,
 *
 * Returns a map of texture handles, one for each texture in the atlas
 * The key is the value in the `name` field. The value is the handle.
 *
 * Since an error in one image usually means an error in all images, we return a single error,
 * not a list of errors.
 */
tl::expected<std::vector<TextureHandle>, ImageError> TextureManager::loadTextureAtlas(
    std::string_view filename, std::vector<TextureAtlasLoad> atlasinfo)
{
    return environ_->loadTextureFromFile(filename).and_then(
        [this, &atlasinfo,
         &filename](auto &&texture) -> tl::expected<std::vector<TextureHandle>, ImageError> {
            std::vector<TextureHandle> res;
            auto depth = texture->data->pitch / texture->data->w;

            auto fnCreateRect = [](auto x, auto y, auto w, auto h) {
                SDL_Rect ret;
                ret.x = x;
                ret.y = y;
                ret.w = w;
                ret.h = h;

                return ret;
            };

            for (const auto &element : atlasinfo) {
                auto dest = make_surface_unique_ptr(
                    SDL_CreateRGBSurface(0, element.width, element.height, depth, 0, 0, 0, 0));
                SDL_Rect srcrect =
                    fnCreateRect(element.x, element.y, element.width, element.height);
                SDL_Rect dstrect = fnCreateRect(0, 0, element.width, element.height);

                if (SDL_BlitSurface(texture->data.get(), &srcrect, dest.get(), &dstrect) < 0) {
                    return tl::make_unexpected(ImageError::TextureAtlasError);
                }

                auto fileencode = fmt::format(
                    "{}?w={}&h={}&x={}&y={}", filename, element.width, element.height, element.x,
                    element.y);
                auto handle = this->addTexture(fileencode, std::move(texture));
                if (handle) {
                    res.push_back(*handle);
                } else {
                    tl::make_unexpected(handle.error());
                }
            }

            return tl::expected<std::vector<TextureHandle>, ImageError>(res);
        });
}

/**
 * Load a texture from memory.
 *
 * Returns a texture handle, basically a hash of the texture information, so
 * we can rapidly detect if we loaded the texture twice
 */
tl::expected<TextureHandle, ImageError> TextureManager::loadTexture(
    std::span<uint8_t> data, size_t width, size_t height, TextureFormat format)
{
    return environ_->loadTextureFromMemory(data, width, height, format)
        .and_then([this, width, height, format, &data](auto &&texture) {
            /// TODO: Use fmt::format()
            /// TODO: take a checksum (a crc32/crc64 is ok) from that file

            char fileinfo[128] = {};
            sprintf(fileinfo, "%zu|%zu|%zu|%zx|", data.size(), width, height, (size_t)format);

            auto handle = hashFilename(fileinfo);

            if (auto texiter = textures_.find(handle); texiter != textures_.end()) {
                auto &log = LoggerService::getLogger();

                log->write(
                    "texture-manager", LogType::Warning,
                    "created texture (aliased as {}) already added", fileinfo);
                texiter->second->refcount++;
                return tl::expected<TextureHandle, ImageError>(handle);
            } else {
                textures_[handle] = std::move(texture);
            }

            return tl::expected<TextureHandle, ImageError>(handle);
        });
}

/**
 * Upload the specified texture
 *
 * Returns nullopt on success, and an error on texture error
 */
std::optional<TextureError> TextureManager::uploadTexture(TextureHandle t)
{
    auto texiter = textures_.find(t);
    if (texiter == textures_.end()) {
        return std::make_optional(TextureError::TextureNotFound);
    }

    auto uploadres = environ_->uploadTexture(*texiter->second.get());
    if (uploadres) {
        return std::nullopt;
    } else {
        return std::make_optional(uploadres.error());
    }
}

/**
 * Unload the specified texture from the GPU, but do not remove it from
 * the memory.
 *
 * This might be useful for otimizations for some systems (the scene manager,
 * for example), but it might not be good for others.
 */
std::optional<TextureError> TextureManager::unloadTexture(TextureHandle t)
{
    auto& log = LoggerService::getLogger();

    auto texiter = textures_.find(t);
    if (texiter == textures_.end()) {
        return std::make_optional(TextureError::TextureNotFound);
    }

    if (texiter->second->texture_unit) {
        log->write("texture-manager", LogType::Error, 
            "cannot unbind: the texture is bound to texture unit %08x",
            *texiter->second->texture_unit);
        return std::make_optional(TextureError::TextureIsBound);
    }

    auto uploadres = environ_->unloadTexture(*texiter->second.get());
    if (uploadres) {
        return std::nullopt;
    } else {
        return std::make_optional(uploadres.error());
    }
}

/**
 * Bind a texture to a certain texture unit
 *
 * A texture unit is simply a space in memory where textures go. If your
 * GPU has 9 texture units, it means that 9 textures can be used simultaneosly,
 * in the same shader.
 *
 * If you bind a texture to a texture unit that had another texture bound,
 * this texture will replace that one.
 */
std::optional<TextureError> TextureManager::bindTexture(TextureHandle t, uint32_t unit)
{
    auto texiter = textures_.find(t);
    if (texiter == textures_.end()) {
        return std::make_optional(TextureError::TextureNotFound);
    }

    auto uploadres = environ_->bindTexture(*texiter->second.get(), unit);
    if (uploadres) {
        texiter->second->texture_unit = std::make_optional(unit);
        return std::nullopt;
    } else {
        return std::make_optional(uploadres.error());
    }
}

/**
 * Query a texture for its size
 *
 * If the texture exists, returns a tuple of values, first the width and then
 * the height.
 *
 * If not, returns std::nullopt
 */
std::optional<std::tuple<size_t, size_t>> TextureManager::getSize(TextureHandle &t)
{
    auto texiter = textures_.find(t);
    if (texiter == textures_.end()) {
        return std::nullopt;
    }

    return std::make_optional(
        std::make_tuple((size_t)texiter->second->data->w, (size_t)texiter->second->data->h));
}

/**
 * Get how many references this texture has
 *
 * This is, more or less, how many people are using it
 *
 * Returns nullopt if the texture is not found, or the amount of
 * references if it is
 */
std::optional<size_t> TextureManager::getReferences(TextureHandle &t)
{
    auto texiter = textures_.find(t);
    if (texiter == textures_.end()) {
        return std::nullopt;
    }

    return std::make_optional((size_t)texiter->second->refcount);
}

/**
 * Unbind a texture from a specified texture unit
 *
 * Returns the renderer-specific texture handle of the texture that was
 * bound to that unit, or 0 if no texture was bound there.
 */
std::optional<TextureError> TextureManager::unbindTexture(unsigned unit)
{
    auto oldtexture = environ_->unbindTexture(unit);
    if (oldtexture) {
        if (*oldtexture == 0) return std::nullopt;

        auto texiter =
            std::find_if(textures_.begin(), textures_.end(), [oldtexture](auto &texpair) {
                return texpair.second->texture_unit && texpair.second->renderer_handle &&
                       *texpair.second->renderer_handle == *oldtexture;
            });

        if (texiter == textures_.end()) {
            return std::make_optional(TextureError::TextureNotFound);
        }

        texiter->second->texture_unit = std::nullopt;
        return std::nullopt;
    } else {
        return std::make_optional(oldtexture.error());
    }
}

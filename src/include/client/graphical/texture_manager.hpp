#pragma once

/***
    Texture manager

    Manages texture creation and upload, a front-end for the texture environments

    Copyright (C) 2016, 2018-2019, 2021 Arthur Mendes.

***/

#include <array>
#include <bit>  // for std::rotr
#include <client/graphical/texture_environment.hpp>
#include <cstdint>
#include <cstring>  //strcmp()
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace familyline::graphics
{
/**
 * Structure for instructions to load a texture from a texture atlas
 *
 * name is the name in the resulting map
 * x and y are the starting positions, width and height is the texture size.
 */
struct TextureAtlasLoad {
    std::string name;
    size_t x, y, width, height;
};

/**
 * Create, delete and manage reference to some texture
 *
 * The texture manager manages also the texture lifetime
 *
 *  Load[1] -> o -> Upload[2] -> o <-> o -> Bind[3]
 *            /|\                 |   /|\     |
 *             |                  |    |  (texture is on
 *             |                  |    |    GPU memory)
 *             |                  |    |      |
 *             ------Unload[5]----     ---Unbind[4]
 *
 * You start with [1], which loads the file
 * Then, you go to [2], to upload the texture to the videocard memory
 * Then, you go to [3], which bind the texture to a texture unit.
 *   Usually, graphical APIs works with texture units, which are more or less
 *   like a pointer to the texture that is being rendered for a certain render
 *   call (glDrawArrays/glDrawElements for opengl).
 * Then, you go to [4], which unbind textures for a certain texture unit
 * Then, you go to [3] again, to rerender, or you go to [5], which removes the
 * texture from the GPU memory.
 *
 */
class TextureManager
{
public:
    explicit TextureManager(std::unique_ptr<TextureEnvironment> environment)
        : environ_(std::move(environment))
    {}


    TextureManager(const TextureManager&) = delete;
    TextureManager(TextureManager&) = delete;

    /**
     * Load a texture from a file.
     *
     * Returns a texture handle, basically a hash of the texture information, so
     * we can rapidly detect if we loaded the texture twice
     */
    tl::expected<TextureHandle, ImageError> loadTexture(std::string_view filename);

    /**
     * Load multiple textures from a texture atlas,
     *
     * Returns a map of texture handles, one for each texture in the atlas
     * The key is the value in the `name` field. The value is the handle.
     *
     * Since an error in one image usually means an error in all images, we return a single error,
     * not a list of errors.
     */
    tl::expected<std::vector<TextureHandle>, ImageError> loadTextureAtlas(
        std::string_view filename, std::vector<TextureAtlasLoad> atlasinfo);

    /**
     * Load a texture from memory.
     *
     * Returns a texture handle, basically a hash of the texture information, so
     * we can rapidly detect if we loaded the texture twice
     */
    tl::expected<TextureHandle, ImageError> loadTexture(
        std::span<uint8_t> data, size_t width, size_t height, TextureFormat format);

    std::tuple<unsigned, unsigned> getTextureMaxSize() const
    {
        return environ_->getTextureMaxSize();
    }

    /**
     * Get raw texture from the texture handle
     *
     * Usually used by the GUI subsystem so it can use the texture
     * manager to store GUI images, and load them in an uniform way.
     */
    std::optional<std::reference_wrapper<Texture>> getRawTexture(TextureHandle handle);

    
    /**
     * Upload the specified texture
     *
     * Returns nullopt on success, and an error on texture error
     */
    std::optional<TextureError> uploadTexture(TextureHandle);

    /**
     * Unload the specified texture from the GPU, but do not remove it from
     * the memory.
     *
     * This might be useful for otimizations for some systems (the scene manager,
     * for example), but it might not be good for others.
     */
    std::optional<TextureError> unloadTexture(TextureHandle);

    /**
     * Bind a texture to a certain texture unit
     *
     * A texture unit is simply a space in memory where textures go. If your
     * GPU has 9 texture units, it means that 9 textures can be used
     * simultaneosly, in the same shader.
     *
     * If you bind a texture to a texture unit that had another texture bound,
     * this texture will replace that one.
     */
    std::optional<TextureError> bindTexture(TextureHandle, uint32_t unit);

    /**
     * Unbind the current texture from a certain texture unit
     */
    std::optional<TextureError> unbindTexture(uint32_t unit);

    /**
     * Query a texture for its size
     *
     * If the texture exists, returns a tuple of values, first the width and then
     * the height.
     *
     * If not, returns std::nullopt
     */
    std::optional<std::tuple<size_t, size_t>> getSize(TextureHandle &);

    /**
     * Get how many references this texture has
     *
     * This is, more or less, how many people are using it
     *
     * Returns nullopt if the texture is not found, or the amount of
     * references if it is
     */
    std::optional<size_t> getReferences(TextureHandle &);

    /**
     * Register the texture here, so you can get it later
     *
     * Note that this is a temporary API, because we will register everything in their
     * asset manager.
     */
    void registerTexture(std::string key, TextureHandle val) { storage_[key] = val; }

    std::optional<TextureHandle> getFromRegistry(std::string key)
    {
        if (!storage_.contains(key)) {
            return std::nullopt;
        } else {
            return std::make_optional<TextureHandle>(storage_[key]);
        }
    }

private:
    tl::expected<TextureHandle, ImageError> addTexture(
        std::string_view filename, std::unique_ptr<Texture> texture);

    std::unique_ptr<TextureEnvironment> environ_;

    uint64_t hashFilename(std::string_view);

    std::unordered_map<TextureHandle, std::unique_ptr<Texture>> textures_;

    std::unordered_map<std::string, TextureHandle> storage_;
};

}  // namespace familyline::graphics

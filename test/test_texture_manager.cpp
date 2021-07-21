
/**
 * Small code proposal for the new texture manager
 */

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

#include <array>
#include <bit>  // for std::rotr
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <tl/expected.hpp>
#include <unordered_map>
#include <vector>

#include "texture_environment.hpp"

class DemoTextureEnvironment : public TextureEnvironment
{
public:
    /**
     * Initialize the environment, Return True on success, False on fail
     */
    virtual bool initialize();

    virtual tl::expected<std::unique_ptr<Texture>, ImageError> loadTextureFromFile(
        std::string_view);

    /**
     * Load a texture from a memory buffer
     *
     * Alongside the memory buffer, you need to specify the width, height and
     * texture color format
     */
    virtual tl::expected<std::unique_ptr<Texture>, ImageError> loadTextureFromMemory(
        std::span<uint8_t> data, size_t width, size_t height, TextureFormat format);

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
    virtual tl::expected<uintptr_t, TextureError> uploadTexture(Texture &);

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
        Texture &, std::span<uint8_t> data, size_t width, size_t height, TextureFormat format);

    /**
     * Unload a texture from the videocard
     *
     * Returns the (now not valid) API-specific texture code on success, or an
     * error on failure
     */
    virtual tl::expected<uintptr_t, TextureError> unloadTexture(Texture &);

    /**
     * Set a certain texture to be the current one in the specified texture unit.
     *
     * This will affect rendering. For example, if your shader needs a texture at
     * texture unit 0, the texture you set to the current one is the one that
     * will be rendered
     */
    virtual tl::expected<uintptr_t, TextureError> bindTexture(Texture &, unsigned unit);

    /**
     * Unbind a texture from a specified texture unit
     */
    virtual tl::expected<uintptr_t, TextureError> unbindTexture(unsigned unit);

    virtual ~DemoTextureEnvironment();

    size_t uploadTexturesCount() { return textures_.size();  }
    bool hasTextureBound(size_t unit) { return current_textures_[unit] > 0; }
    
private:
    bool started_ = false;

    std::array<uint32_t /* textures */, 9 /* maxunits */> current_textures_ = {0, 0, 0, 0, 0,
                                                                               0, 0, 0, 0};

    std::unordered_map<uint32_t, SDL_Surface *> textures_;
};

bool DemoTextureEnvironment::initialize()
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
std::tuple<unsigned, unsigned> DemoTextureEnvironment::getTextureMaxSize() const
{
    return std::make_tuple(2048, 2048);
}

/**
 * Create an unique ptr of a SDL_Surface
 *
 * Useful so we do not need to worry about managing it later
 */
surface_unique_ptr make_surface_unique_ptr(SDL_Surface *s)
{
    return std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>(s, &SDL_FreeSurface);
}

tl::expected<std::unique_ptr<Texture>, ImageError> DemoTextureEnvironment::loadTextureFromFile(
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
tl::expected<std::unique_ptr<Texture>, ImageError> DemoTextureEnvironment::loadTextureFromMemory(
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
tl::expected<uintptr_t, TextureError> DemoTextureEnvironment::uploadTexture(Texture &t)
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
tl::expected<uintptr_t, ImageError> DemoTextureEnvironment::setTextureData(
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
tl::expected<uintptr_t, TextureError> DemoTextureEnvironment::unloadTexture(Texture &t)
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
tl::expected<uintptr_t, TextureError> DemoTextureEnvironment::bindTexture(Texture &t, unsigned unit)
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
tl::expected<uintptr_t, TextureError> DemoTextureEnvironment::unbindTexture(unsigned unit)
{
    if (unit >= current_textures_.size())
        return tl::make_unexpected(TextureError::TextureUnitInvalid);

    auto handle             = current_textures_[unit];
    current_textures_[unit] = 0;

    return handle;
}

DemoTextureEnvironment::~DemoTextureEnvironment()
{
    if (started_) {
        IMG_Quit();
    }
}

///////////////////////////

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
    TextureManager(std::unique_ptr<TextureEnvironment> &&environ) : environ_(std::move(environ)) {}

    /**
     * Load a texture from a file.
     *
     * Returns a texture handle, basically a hash of the texture information, so
     * we can rapidly detect if we loaded the texture twice
     */
    tl::expected<TextureHandle, ImageError> loadTexture(std::string_view filename);

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

private:
    std::unique_ptr<TextureEnvironment> environ_;

    uint64_t hashFilename(std::string_view);

    std::unordered_map<TextureHandle, std::unique_ptr<Texture>> textures_;
};

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

/**
 * Load a texture.
 *
 * Returns a texture handle, basically a hash of the texture information, so
 * we can rapidly detect if we loaded the texture twice
 */
tl::expected<TextureHandle, ImageError> TextureManager::loadTexture(std::string_view filename)
{
    return environ_->loadTextureFromFile(filename).and_then([this, &filename](auto &&texture) {
        auto handle = hashFilename(filename);

        if (auto texiter = textures_.find(handle); texiter != textures_.end()) {
            printf("texture %s already added\n", filename.data());
            texiter->second->refcount++;
            return tl::expected<TextureHandle, ImageError>(handle);
        } else {
            textures_[handle] = std::move(texture);
        }

        return tl::expected<TextureHandle, ImageError>(handle);
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
                printf("created texture (aliased as %s) already added\n", fileinfo);
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
    auto texiter = textures_.find(t);
    if (texiter == textures_.end()) {
        return std::make_optional(TextureError::TextureNotFound);
    }

    if (texiter->second->texture_unit) {
        printf("cannot unbind: the texture is bound to texture unit %08x", *texiter->second->texture_unit);
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

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "utils.hpp"

TEST(TextureManagerText, LoadTexture)
{
    auto manager = std::make_unique<TextureManager>(std::make_unique<DemoTextureEnvironment>());

    auto handle = manager->loadTexture(TESTS_DIR "/textest.png");
    ASSERT_TRUE(handle);
    ASSERT_NE(0, *handle);
}

TEST(TextureManagerTest, LoadUnexistingTexture)
{
    auto manager = std::make_unique<TextureManager>(std::make_unique<DemoTextureEnvironment>());

    auto handle = manager->loadTexture(TESTS_DIR "/communism_defects.png");
    ASSERT_FALSE(handle);
    ASSERT_EQ(ImageError::ImageNotExists, handle.error());
}

TEST(TextureManagerTest, LoadTextureAndGetCorrectSize)
{
    auto manager = std::make_unique<TextureManager>(std::make_unique<DemoTextureEnvironment>());

    auto handle = manager->loadTexture(TESTS_DIR "/texture256x192.png");
    ASSERT_TRUE(handle);
    auto [width, height] = manager->getSize(*handle).value_or(std::make_tuple(0, 0));
    EXPECT_EQ(256, width);
    EXPECT_EQ(192, height);
}

TEST(TextureManagerTest, LoadTextureAndCheckIfHandlesAreNotEqual)
{
    auto manager = std::make_unique<TextureManager>(std::make_unique<DemoTextureEnvironment>());

    auto handle1 = manager->loadTexture(TESTS_DIR "/textest.png");
    auto handle2 = manager->loadTexture(TESTS_DIR "/texture256x192.png");
    ASSERT_TRUE(handle1);
    ASSERT_TRUE(handle2);
    ASSERT_NE(*handle1, *handle2);
}

TEST(TextureManagerTest, CheckIfSameFilesHaveSameHandles)
{
    auto manager = std::make_unique<TextureManager>(std::make_unique<DemoTextureEnvironment>());

    auto handle1 = manager->loadTexture(TESTS_DIR "/textest.png");
    auto handle2 = manager->loadTexture(TESTS_DIR "/texture256x192.png");
    auto handle3 = manager->loadTexture(TESTS_DIR "/textest.png");
    ASSERT_TRUE(handle1);
    ASSERT_TRUE(handle2);
    ASSERT_TRUE(handle3);
    ASSERT_NE(*handle1, *handle2);
    ASSERT_NE(*handle3, *handle2);
    ASSERT_EQ(*handle1, *handle3);
}

TEST(TextureManagerTest, CheckIfTextureUploadsToVideocard)
{
    auto tenv = std::make_unique<DemoTextureEnvironment>();
    auto tenvptr = tenv.get();
    auto manager = std::make_unique<TextureManager>(std::move(tenv));

    EXPECT_EQ(0, tenvptr->uploadTexturesCount());
    
    auto handle = manager->loadTexture(TESTS_DIR "/texture256x192.png");
    ASSERT_TRUE(handle);
    EXPECT_EQ(0, tenvptr->uploadTexturesCount());
    
    auto uploadres = manager->uploadTexture(*handle);
    ASSERT_FALSE(uploadres);
    EXPECT_EQ(1, tenvptr->uploadTexturesCount());


}

TEST(TextureManagerTest, CheckIfTextureUploadsToVideocardAndThenRemoves)
{
    auto tenv = std::make_unique<DemoTextureEnvironment>();
    auto tenvptr = tenv.get();
    auto manager = std::make_unique<TextureManager>(std::move(tenv));

    EXPECT_EQ(0, tenvptr->uploadTexturesCount());
    
    auto handle = manager->loadTexture(TESTS_DIR "/texture256x192.png");
    ASSERT_TRUE(handle);
    EXPECT_EQ(0, tenvptr->uploadTexturesCount());
    
    auto uploadres = manager->uploadTexture(*handle);
    EXPECT_EQ(1, tenvptr->uploadTexturesCount());
    ASSERT_FALSE(uploadres);

    uploadres = manager->unloadTexture(*handle);
    if (uploadres) {
        printf("failed to upload devasso.png - ");
        switch (*uploadres) {
        case TextureError::InsufficientMemory:
            printf("unsufficient memory");
            break;
        case TextureError::TextureNotFound:
            printf("texture not found");
            break;
        case TextureError::TextureIsBound:
            printf("texture is currently bound to an unit");
            break;
        case TextureError::TextureAlreadyThere:
            printf("texture uploaded");
            break;
        case TextureError::UnknownError:
            printf("unknown error");
            break;
        }
        puts("");
    } else {
        printf("devasso.png uploaded!\n");
    }

    EXPECT_EQ(0, tenvptr->uploadTexturesCount());
    ASSERT_FALSE(uploadres);
}


TEST(TextureManagerTest, CheckIfTextureUploadsToVideocardAndBinds)
{
    auto tenv = std::make_unique<DemoTextureEnvironment>();
    auto tenvptr = tenv.get();
    auto manager = std::make_unique<TextureManager>(std::move(tenv));

    EXPECT_EQ(0, tenvptr->uploadTexturesCount());
    
    auto handle = manager->loadTexture(TESTS_DIR "/texture256x192.png");
    ASSERT_TRUE(handle);
    EXPECT_EQ(0, tenvptr->uploadTexturesCount());
    
    auto uploadres = manager->uploadTexture(*handle);
    ASSERT_FALSE(uploadres);
    EXPECT_EQ(1, tenvptr->uploadTexturesCount());

    uploadres = manager->bindTexture(*handle, 1);
    ASSERT_FALSE(uploadres);
    EXPECT_FALSE(tenvptr->hasTextureBound(0));
    EXPECT_TRUE(tenvptr->hasTextureBound(1)); 
    EXPECT_FALSE(tenvptr->hasTextureBound(2));   
}

TEST(TextureManagerTest, CheckIfTextureUploadsToVideocardBindsAndUnbinds)
{
    auto tenv = std::make_unique<DemoTextureEnvironment>();
    auto tenvptr = tenv.get();
    auto manager = std::make_unique<TextureManager>(std::move(tenv));

    EXPECT_EQ(0, tenvptr->uploadTexturesCount());
    
    auto handle = manager->loadTexture(TESTS_DIR "/texture256x192.png");
    ASSERT_TRUE(handle);
    EXPECT_EQ(0, tenvptr->uploadTexturesCount());
    
    auto uploadres = manager->uploadTexture(*handle);
    ASSERT_FALSE(uploadres);
    EXPECT_EQ(1, tenvptr->uploadTexturesCount());

    uploadres = manager->bindTexture(*handle, 1);
    ASSERT_FALSE(uploadres);
    EXPECT_FALSE(tenvptr->hasTextureBound(0));
    EXPECT_TRUE(tenvptr->hasTextureBound(1)); 
    EXPECT_FALSE(tenvptr->hasTextureBound(2));

    uploadres = manager->unbindTexture(1);
    ASSERT_FALSE(uploadres);
    EXPECT_FALSE(tenvptr->hasTextureBound(0));
    EXPECT_FALSE(tenvptr->hasTextureBound(1)); 
    EXPECT_FALSE(tenvptr->hasTextureBound(2));   
}


TEST(TextureManagerTest, CheckIfTextureCanBeCreatedFromBuffer)
{
    auto manager = std::make_unique<TextureManager>(std::make_unique<DemoTextureEnvironment>());

    // 4x4 pixel grid test.
    // first column red, then green, then blue, then white.
    // clang-format off
    std::array<uint8_t, 4*4*4> rgbatestpixel = {
        0xff, 0x00, 0x00, 0xff,
        0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,

        0xff, 0x00, 0x00, 0xff,
        0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,

        0xff, 0x00, 0x00, 0xff,
        0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff,

        0xff, 0x00, 0x00, 0xff,
        0x00, 0xff, 0x00, 0xff,
        0x00, 0x00, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff
    };
    // clang-format on

    auto createdhandle =
        manager->loadTexture(rgbatestpixel, 4, 4, TextureFormat::RGBA32);

    ASSERT_TRUE(createdhandle);
    auto [width, height] = manager->getSize(*createdhandle).value_or(std::make_tuple(0, 0));
    EXPECT_EQ(4, width);
    EXPECT_EQ(4, height);
}

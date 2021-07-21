
/**
 * Small code proposal for the new texture manager
 */

#include <gtest/gtest.h>

#include "utils/test_texenv.hpp"

#include <client/graphical/texture_manager.hpp>
#include <client/graphical/texture_environment.hpp>

using namespace familyline::graphics;

TEST(TextureManagerText, LoadTexture)
{
    auto manager = std::make_unique<TextureManager>(std::make_unique<TestTextureEnvironment>());

    auto handle = manager->loadTexture(TESTS_DIR "/textest.png");
    ASSERT_TRUE(handle);
    ASSERT_NE(0, *handle);
}

TEST(TextureManagerTest, LoadUnexistingTexture)
{
    auto manager = std::make_unique<TextureManager>(std::make_unique<TestTextureEnvironment>());

    auto handle = manager->loadTexture(TESTS_DIR "/communism_defects.png");
    ASSERT_FALSE(handle);
    ASSERT_EQ(ImageError::ImageNotExists, handle.error());
}

TEST(TextureManagerTest, LoadTextureAndGetCorrectSize)
{
    auto manager = std::make_unique<TextureManager>(std::make_unique<TestTextureEnvironment>());

    auto handle = manager->loadTexture(TESTS_DIR "/texture256x192.png");
    ASSERT_TRUE(handle);
    auto [width, height] = manager->getSize(*handle).value_or(std::make_tuple(0, 0));
    EXPECT_EQ(256, width);
    EXPECT_EQ(192, height);
}

TEST(TextureManagerTest, LoadTextureAndCheckIfHandlesAreNotEqual)
{
    auto manager = std::make_unique<TextureManager>(std::make_unique<TestTextureEnvironment>());

    auto handle1 = manager->loadTexture(TESTS_DIR "/textest.png");
    auto handle2 = manager->loadTexture(TESTS_DIR "/texture256x192.png");
    ASSERT_TRUE(handle1);
    ASSERT_TRUE(handle2);
    ASSERT_NE(*handle1, *handle2);
}

TEST(TextureManagerTest, CheckIfSameFilesHaveSameHandles)
{
    auto manager = std::make_unique<TextureManager>(std::make_unique<TestTextureEnvironment>());

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
    auto tenv = std::make_unique<TestTextureEnvironment>();
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
    auto tenv = std::make_unique<TestTextureEnvironment>();
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
    auto tenv = std::make_unique<TestTextureEnvironment>();
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
    auto tenv = std::make_unique<TestTextureEnvironment>();
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
    auto manager = std::make_unique<TextureManager>(std::make_unique<TestTextureEnvironment>());

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

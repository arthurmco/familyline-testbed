#include <gtest/gtest.h>

#include <common/logger.hpp>
#include <common/logic/terrain.hpp>
#include <common/logic/terrain_file.hpp>
#include <optional>
#include <string>

#include "utils.hpp"

using namespace familyline;
using namespace familyline::logic;

class TerrainService
{
};

TEST(TerrainTest, TestTerrainOpen)
{
    LoggerService::createLogger();

    TerrainFile tf;
    ASSERT_TRUE(tf.open(TESTS_DIR "/terrain_test.flte"));

    ASSERT_STREQ("Test", tf.getName().data());
    ASSERT_STREQ("Test terrain", tf.getDescription().data());
    ASSERT_EQ(1, tf.getAuthors().size());
    ASSERT_STREQ("Arthur Mendes <arthurmco@gmail.com>", tf.getAuthors()[0].c_str());

    auto [width, height] = tf.getSize();
    ASSERT_EQ(512, width);
    ASSERT_EQ(512, height);
}

TEST(TerrainTest, TestTerrainDoNotOpenBrokenCRC)
{
    LoggerService::createLogger();

    TerrainFile tf;
    ASSERT_FALSE(tf.open(TESTS_DIR "/terrain_test_brokencrc.flte"));
}

TEST(TerrainTest, TestTerrainDoNotOpenBrokenTerrain)
{
    LoggerService::createLogger();

    TerrainFile tf;
    ASSERT_FALSE(tf.open(TESTS_DIR "/terrain_test_brokenterrain.flte"));

    ASSERT_EQ(0, tf.getName().size());
}

TEST(TerrainTest, TestTerrainCoordConversion)
{
    LoggerService::createLogger();

    TerrainFile tf;
    tf.open(TESTS_DIR "/terrain_test.flte");

    Terrain t{tf};
    glm::vec3 c0(32.0, 16.0, 32.0);
    auto gc0 = t.gameToGraphical(c0);

    ASSERT_FLOAT_EQ(16.0, gc0.x);
    ASSERT_FLOAT_EQ(0.16, gc0.y);
    ASSERT_FLOAT_EQ(16.0, gc0.z);

    auto rgc0 = t.graphicalToGame(gc0);
    ASSERT_FLOAT_EQ(32.0, rgc0.x);
    ASSERT_FLOAT_EQ(16.0, rgc0.y);
    ASSERT_FLOAT_EQ(32.0, rgc0.z);

    ASSERT_FLOAT_EQ(c0.x, rgc0.x);
    ASSERT_FLOAT_EQ(c0.y, rgc0.y);
    ASSERT_FLOAT_EQ(c0.z, rgc0.z);
}

TEST(TerrainTest, TestOverlayCreation)
{
    LoggerService::createLogger();

    TerrainFile tf;
    tf.open(TESTS_DIR "/terrain_test.flte");

    Terrain t{tf};
    auto [width, height] = t.getSize();

    auto ovl = t.createOverlay("testoverlay");
    ASSERT_EQ(width * height, ovl->getData().size());
}

TEST(TerrainTest, TestHeightRetrieve)
{
    LoggerService::createLogger();

    TerrainFile tf;
    tf.open(TESTS_DIR "/terrain_test.flte");

    Terrain t{tf};
    auto [width, height] = t.getSize();

    ASSERT_EQ(48, t.getHeightFromCoords(glm::vec2(10, 10)));
    ASSERT_EQ(48, t.getHeightFromCoords(glm::vec2(10, 15)));
    ASSERT_EQ(51, t.getHeightFromCoords(glm::vec2(315, 10)));
    ASSERT_EQ(48, t.getHeightFromCoords(glm::vec2(15, 10)));
    ASSERT_EQ(48, t.getHeightFromCoords(glm::vec2(20, 20)));
    ASSERT_EQ(49, t.getHeightFromCoords(glm::vec2(120, 23)));
    ASSERT_EQ(0xfd, t.getHeightFromCoords(glm::vec2(240, 240)));
    ASSERT_EQ(0xfd, t.getHeightFromCoords(glm::vec2(250, 234)));
    ASSERT_EQ(0x5f, t.getHeightFromCoords(glm::vec2(508, 505)));
}

#include <gtest/gtest.h>

#include <common/logger.hpp>
#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <unordered_map>

#include "terrain_file.hpp"
#include "utils.hpp"

using namespace familyline;
using namespace familyline::logic;

#include <algorithm>

/**
 * A nice way to bind additional data into the terrain, usually data
 * that has a value per slot, like pathfinding data, fog of war or, in
 * some other games, data like crime, pollution...
 *
 * The only difference is that the terrain owns this data instead of you,
 * so is one less thing to worry about.
 */
class TerrainOverlay
{
private:
    std::vector<uint16_t> data_;

public:
    TerrainOverlay(size_t size);

    decltype(data_)& getData() { return data_; }
};

TerrainOverlay::TerrainOverlay(size_t size) : data_(std::vector<uint16_t>(size, 0)) {}

///////////////////////////////////

/**
 * Terrain types.
 *
 * Under development
 */
enum TerrainType { Grass = 0, Dirt = 10, Water = 20, Mountain = 30 };

class Terrain
{
private:
    TerrainFile& tf_;

    double xzscale_ = 0.50;
    double yscale_  = 0.01;

    std::unordered_map<std::string, std::unique_ptr<TerrainOverlay>> overlays_;

public:
    Terrain(TerrainFile& tf) : tf_(tf) {}

    /**
     * Convert game engine coordinates to opengl coordinates
     */
    glm::vec3 gameToGraphical(glm::vec3 gameCoords)
    {
        return glm::vec3(gameCoords.x * xzscale_, gameCoords.y * yscale_, gameCoords.z * xzscale_);
    }

    /**
     * Get height coords from a set of X and Y coords in the map
     */
    unsigned getHeightFromCoords(glm::vec2 coords);

    /**
     * Convert opengl coordinates to game engine coordinates
     */
    glm::vec3 graphicalToGame(glm::vec3 gfxcoords)
    {
        return glm::vec3(gfxcoords.x / xzscale_, gfxcoords.y / yscale_, gfxcoords.z / xzscale_);
    }

    std::tuple<uint32_t, uint32_t> getSize() { return tf_.getSize(); }

    TerrainOverlay* createOverlay(const char* name);
};

/**
 * Get height coords from a set of X and Y coords in the map
 */
unsigned ::Terrain::getHeightFromCoords(glm::vec2 coords)
{
    auto [w, h] = tf_.getSize();
    auto idx    = coords.y * w + coords.x;

    auto& data = tf_.getHeightData();
    return data[idx];
}

TerrainOverlay* ::Terrain::createOverlay(const char* name)
{
    std::string n{name};
    auto [w, h] = tf_.getSize();

    overlays_[n] = std::make_unique<TerrainOverlay>(w * h);

    auto& log = LoggerService::getLogger();
    log->write("terrain", LogType::Debug, "overlay '%s' created", name);

    return overlays_[n].get();
}

///////////////////////////////////

bool TerrainFile::checkFileCRC(uint32_t expectedCRC, FILE* f)
{
    return this->checkCRC(expectedCRC, f, 0, offsetof(TerrainFileHeader, file_crc32));
}

/**
 * See if the CRC terrain match with the one you pass as a parameter
 */
bool TerrainFile::checkTerrainCRC(uint32_t expectedCRC, FILE* f, uint32_t header_off)
{
    return this->checkCRC(expectedCRC, f, header_off, offsetof(TerrainHeader, terrain_crc32));
}

bool TerrainFile::checkCRC(uint32_t expectedCRC, FILE* f, uint32_t start, uint32_t crc_field_off)
{
    auto oldpos = ftell(f);

    fseek(f, 0L, SEEK_END);
    auto filesize = ftell(f);

    fseek(f, start, SEEK_SET);
    char* filedata = new char[filesize];
    auto reallen   = fread(filedata, 1, filesize, f);

    filedata[crc_field_off]     = 0;
    filedata[crc_field_off + 1] = 0;
    filedata[crc_field_off + 2] = 0;
    filedata[crc_field_off + 3] = 0;

    unsigned long crc = crc32(0L, Z_NULL, 0);
    crc               = crc32(crc, (const unsigned char*)filedata, reallen);

    delete[] filedata;

    auto& log = LoggerService::getLogger();
    log->write("terrain-file", LogType::Debug, "crc32 %#x x %#x", crc, expectedCRC);

    fseek(f, oldpos, SEEK_SET);
    return (crc == expectedCRC);
}

/**
 * Read the terrain header, return the TerrainHeader structure
 *
 */
std::optional<TerrainHeader> TerrainFile::readTerrainHeader(FILE* f, uint32_t header_off)
{
    auto& log = LoggerService::getLogger();

    TerrainHeader th;
    fseek(fTerrain_, header_off, SEEK_SET);
    auto thsize = fread((TerrainHeader*)&th, sizeof(th), 1, fTerrain_);

    if (thsize < 1) {
        log->write(
            "terrain-file", LogType::Error, "file size is too small, could not read terrain header",
            thsize);
        return std::nullopt;
    }

    if (th.magic != this->terrain_magic) {
        log->write("terrain-file", LogType::Error, "terrain header magic number is wrong");
        return std::nullopt;
    }

    if (!this->checkTerrainCRC(th.terrain_crc32, f, header_off)) {
        log->write("terrain-file", LogType::Error, "terrain CRC32 checksum is wrong");
        return std::nullopt;
    }

    return std::make_optional(th);
}

/**
 * Read the file header, return the offset to the terrain header.
 *
 * If the offset could not be read, return 0, since 0 will never be a
 * valid terrain header offset.
 */
uint32_t TerrainFile::readFileHeader(FILE* f)
{
    auto& log = LoggerService::getLogger();

    TerrainFileHeader tfh;
    auto tfhsize = fread((TerrainFileHeader*)&tfh, sizeof(tfh), 1, f);
    if (tfhsize < 1) {
        log->write(
            "terrain-file", LogType::Error, "file size is too small, could not read file header",
            tfhsize);
        return 0;
    }

    if (tfh.magic != this->file_magic) {
        log->write("terrain-file", LogType::Error, "file header magic is wrong");
        return 0;
    }

    if (!this->checkFileCRC(tfh.file_crc32, f)) {
        log->write("terrain-file", LogType::Error, "file CRC32 checksum is wrong");
        return 0;
    }

    return tfh.terrain_off;
}

/**
 * Read the terrain height and type data.
 *
 * Return true if it could, false if it could not
 */
bool TerrainFile::readTerrainData(FILE* f, TerrainHeader& th)
{
    auto& log = LoggerService::getLogger();

    auto gridsize = th.width * th.height;
    height_data.reserve(gridsize);
    type_data.reserve(gridsize);

    fseek(f, th.terrain_data_off, SEEK_SET);
    auto heightsize = fread(height_data.data(), sizeof(uint16_t), gridsize, f);

    fseek(f, th.terrain_type_off, SEEK_SET);
    auto typesize = fread(type_data.data(), sizeof(uint16_t), gridsize, f);

    if (heightsize < gridsize || typesize < gridsize) {
        log->write("terrain-file", LogType::Error, "terrain data does not match the terrain size");

        height_data.clear();
        type_data.clear();
        return false;
    }

    return true;
}

/**
 * Read a pascal string (a string prefixed by its length) from the file,
 * in the current position
 *
 * The name and each one of the authors are stored this way
 */
std::string TerrainFile::readPascalString(FILE* f)
{
    auto len = fgetc(f);

    char s[len + 1];

    fread(s, sizeof(s[0]), len, f);
    s[len] = '\0';
    return std::string{s};
}

std::string TerrainFile::readName(FILE* f, const TerrainHeader& th)
{
    if (th.name_off < sizeof(TerrainFileHeader)) {
        return "";
    }

    fseek(f, th.name_off, SEEK_SET);
    return readPascalString(f);
}

std::string TerrainFile::readDescription(FILE* f, const TerrainHeader& th)
{
    if (th.desc_off < sizeof(TerrainFileHeader)) {
        return "";
    }

    fseek(f, th.desc_off, SEEK_SET);
    auto len = fgetc(f);
    len |= (fgetc(f) << 8);

    char s[len + 1];
    fread(s, sizeof(s[0]), len, f);
    s[len] = '\0';

    return std::string{s};
}

std::vector<std::string> TerrainFile::readAuthors(FILE* f, const TerrainHeader& th)
{
    if (th.author_off < sizeof(TerrainFileHeader)) {
        return std::vector<std::string>();
    }

    fseek(f, th.author_off, SEEK_SET);

    std::vector<std::string> auths;
    auto authcount = fgetc(f);
    for (auto i = 0; i < authcount; i++) {
        auths.push_back(readPascalString(f));
    }

    return auths;
}

bool TerrainFile::open(std::string_view path)
{
    height_data.clear();
    type_data.clear();

    auto& log = LoggerService::getLogger();

    fTerrain_ = fopen(path.data(), "rb");
    if (!fTerrain_) {
        log->write("terrain-file", LogType::Error, "file %s not found", path.data());
        return false;
    }

    auto th_offset = this->readFileHeader(fTerrain_);
    if (th_offset == 0) {
        log->write("terrain-file", LogType::Error, "could not read %s", path.data());
        return false;
    }

    auto th = this->readTerrainHeader(fTerrain_, th_offset);
    if (!th) {
        log->write(
            "terrain-file", LogType::Error, "error while reading terrain data from file %s",
            path.data());
        return false;
    }

    name_        = readName(fTerrain_, *th);
    description_ = readDescription(fTerrain_, *th);
    authors_     = readAuthors(fTerrain_, *th);
    size_        = std::make_tuple(th->width, th->height);

    if (!this->readTerrainData(fTerrain_, *th)) {
        log->write(
            "terrain-file", LogType::Error, "error while reading terrain content from file %s",
            path.data());
        name_.clear();
        description_.clear();
        authors_.clear();
        size_ = std::make_tuple(0, 0);
        return false;
    }

    fclose(fTerrain_);

    std::string authlist;
    for (auto& a : authors_) {
        authlist.append(a);
        authlist.append(", ");
    }

    if (authors_.size() > 0) authlist = authlist.substr(0, authlist.size() - 2);

    log->write("terrain-file", LogType::Info, "read terrain %s successfully", path.data());
    log->write("terrain-file", LogType::Info, "\tname: %s", name_.c_str());
    log->write("terrain-file", LogType::Info, "\tdescription: %s", description_.c_str());
    log->write("terrain-file", LogType::Info, "\tauthors: %s", authlist.c_str());

    return true;
}

/////////////////////////////

class TerrainRenderer
{
};

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

    ::Terrain t{tf};
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

    ::Terrain t{tf};
    auto [width, height] = t.getSize();

    auto ovl = t.createOverlay("testoverlay");
    ASSERT_EQ(width * height, ovl->getData().size());
}

TEST(TerrainTest, TestHeightRetrieve)
{
    LoggerService::createLogger();

    TerrainFile tf;
    tf.open(TESTS_DIR "/terrain_test.flte");

    ::Terrain t{tf};
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

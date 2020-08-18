#include <zlib.h>

#include <common/logger.hpp>
#include <common/logic/terrain_file.hpp>

using namespace familyline;
using namespace familyline::logic;

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
    height_data = std::vector<uint16_t>(gridsize, 0);
    type_data = std::vector<uint16_t>(gridsize, 0);

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

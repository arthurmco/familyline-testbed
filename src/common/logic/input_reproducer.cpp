#include <input_serialize_generated.h>

#include <cerrno>
#include <cinttypes>
#include <common/logger.hpp>
#include <common/logic/input_file.hpp>
#include <common/logic/input_reproducer.hpp>
#include <common/logic/player_actions.hpp>
#include <common/logic/player_manager.hpp>
#include <cstring>

using namespace familyline::logic;

/**
 * Read player info at the file's current position
 */
std::vector<familyline::logic::InputInfo> readPlayerInfo(FILE* file)
{
    using namespace familyline;

    auto& log = LoggerService::getLogger();

    uint32_t size = 0;
    fread((void*)&size, 1, sizeof(size), file);

    char* data = new char[size + 1];
    auto rsize = fread((void*)data, 1, size, file);

    if (size != rsize) {
        log->write(
            "input-reproducer", LogType::Error, "Could not read the player info section (%d of %d)",
            rsize, size);
        return {};
    }

    flatbuffers::FlatBufferBuilder builder;

    auto playerinfo = flatbuffers::GetRoot<PlayerInfoChunk>((uint8_t*)data);

    std::vector<logic::InputInfo> players;
    for (auto vit = playerinfo->players()->cbegin(); vit != playerinfo->players()->cend(); ++vit) {
        log->write(
            "input-reproducer", LogType::Info, "found player (name %s, id %" PRIx64 ", color %s)",
            vit->name()->c_str(), vit->id(), vit->color()->c_str());

        players.push_back(
            logic::InputInfo{vit->name()->str(), vit->color()->str(), vit->id(), {}, {}});
    }

    return players;
}

long long int readInputCount(FILE* file)
{
    using namespace familyline;

    auto& log = LoggerService::getLogger();

    auto loc = ftell(file);

    uint32_t icount      = 0;
    unsigned footerstart = 12;  // MAGIC + inputcount + checksum

    fseek(file, 0, SEEK_END);
    auto end = ftell(file);

    fseek(file, end - footerstart, SEEK_SET);
    char magic[5] = {};

    fread((void*)magic, 4, 1, file);
    if (strncmp(magic, R_FOOTER_MAGIC, 4)) {
        log->write(
            "input-reproducer", LogType::Error, "Wrong footer section magic (%s != %s)", magic,
            R_FOOTER_MAGIC);
        return -1;
    }

    fread((void*)&icount, 1, sizeof(icount), file);

    fseek(file, loc, SEEK_SET);
    return (long long int)(icount & 0xffffffff);
}

/**
 * Open and verify the file
 *
 * It will return true on success, and false on error
 */
bool InputReproducer::open()
{
    auto& log = LoggerService::getLogger();

    f_ = fopen(file_.data(), "rb");
    if (!f_) {
        f_ = nullptr;
        log->write(
            "input-reproducer", LogType::Error, "Could not open the input file %s (error %d: %s)",
            file_.data(), errno, strerror(errno));

        return false;
    }

    char magic[5]    = {};
    uint32_t version = 0;
    fread((void*)magic, 1, 4, f_);
    fread((void*)&version, 1, 4, f_);

    if (strncmp(magic, R_MAGIC, 4)) {
        log->write(
            "input-reproducer", LogType::Error, "Invalid magic value for file %s (FREC != %s)",
            file_.data(), magic);

        return false;
    }

    if (version != R_VERSION) {
        log->write(
            "input-reproducer", LogType::Error, "Incompatible file version file %s (%x != %x)",
            file_.data(), R_VERSION, version);

        return false;
    }

    pinfo_ = readPlayerInfo(f_);
    if (pinfo_.size() == 0) {
        return false;
    }

    inputcount_ = readInputCount(f_);
    if (inputcount_ < 0) {
        return false;
    }

    log->write(
        "input-reproducer", LogType::Info, "'%s': %lld input actions detected", file_.data(),
        inputcount_);

    return true;
}

/**
 * Create a player session with players that will
 * reproduce what happened in the file
 *
 * The diplomacy will be kept neutral between everyone, but the
 * correct thing to do is to save it into the file
 */
PlayerSession InputReproducer::createPlayerSession() {}

InputReproducer::~InputReproducer()
{
    if (f_) {
        fclose(f_);
    }
}

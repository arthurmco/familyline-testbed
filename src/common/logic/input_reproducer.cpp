#include <input_serialize_generated.h>

#include <algorithm>
#include <cerrno>
#include <cinttypes>
#include <common/logger.hpp>
#include <common/logic/input_file.hpp>
#include <common/logic/input_reproducer.hpp>
#include <common/logic/player_actions.hpp>
#include <common/logic/player_manager.hpp>
#include <cstring>
#include <optional>

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

        delete[] data;
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

    delete[] data;
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

    actioncount_ = readInputCount(f_);
    if (actioncount_ < 0) {
        return false;
    }

    log->write(
        "input-reproducer", LogType::Info, "'%s': %lld input actions detected", file_.data(),
        actioncount_);

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

/**
 * Get the next action from the file
 *
 * If no more actions exist, returns an empty optional
 */
std::optional<PlayerInputAction> InputReproducer::getNextAction()
{
    if (currentaction_ == actioncount_) return std::nullopt;

    // action structure: string "FINP" + int containg the size + flatbuffer data
    // of the input action.
    auto& log = LoggerService::getLogger();

    auto apos = ftell(f_);

    char magic[5]       = {};
    uint32_t actionsize = 0;

    fread((void*)magic, 4, 1, f_);
    if (strncmp(magic, R_ACTION_MAGIC, 4)) {
        log->write(
            "input-reproducer", LogType::Fatal,
            "'%s': Action number %d (at offset %08x) is invalid", file_.data(), currentaction_,
            apos);

        return std::nullopt;
    }

    fread((void*)&actionsize, sizeof(actionsize), 1, f_);
    if (actionsize == 0) {
        log->write(
            "input-reproducer", LogType::Fatal,
            "'%s': Action number %d (at offset %08x) has no size. Stopping because this is very "
            "weird",
            file_.data(), currentaction_, apos);

        return std::nullopt;
    }

    char* data = new char[actionsize + 1];
    auto rsize = fread((void*)data, 1, actionsize, f_);

    flatbuffers::FlatBufferBuilder builder;
    auto actioninfo = flatbuffers::GetRoot<InputElement>((uint8_t*)data);

    PlayerInputType type;
    switch (actioninfo->type_type()) {
        case InputType_cmd: {
            auto cmd = actioninfo->type_as_cmd();
            auto val = std::monostate{};

            switch (cmd->args()->args()->size()) {
            case 1:
                type = CommandInput{cmd->command()->str(), cmd->args()->args()->Get(0)};
                break;
            case 2:
                type = CommandInput{cmd->command()->str(),
                    std::array<int, 2>{
                        (int)cmd->args()->args()->Get(0),
                        (int)cmd->args()->args()->Get(1)
                                    }};
                break;
            default:
                type = CommandInput{cmd->command()->str(), std::monostate{}};
                break;

            }
            
        } break;
        case InputType_sel: {
            auto sel = actioninfo->type_as_sel();
            std::vector<long unsigned int> objects;

            std::copy(
                sel->objects()->values()->cbegin(), sel->objects()->values()->cend(),
                std::back_inserter(objects));
            type = SelectAction{objects};
        } break;
        case InputType_obj_move: {
            auto omove    = actioninfo->type_as_obj_move();
            int xPos = (int)omove->x_pos(), yPos = (int)omove->y_pos();

            type = ObjectMove{xPos, yPos};
        } break;
        case InputType_cam_move: {
            auto cmove = actioninfo->type_as_cam_move();
            double dX = cmove->x_delta(), dY = cmove->y_delta(), dZoom = cmove->zoom_delta();

            type = CameraMove{dX, dY, dZoom};
        } break;
        case InputType_cam_rotate: {
            auto crot = actioninfo->type_as_cam_rotate();
            double radians = crot->radians();

            type = CameraRotate{radians};
        } break;
        case InputType_create: {
            auto centity = actioninfo->type_as_create();
            std::string etype = centity->type()->str();
            int xPos = centity->x_pos(), yPos = centity->y_pos();

            type = CreateEntity{etype, xPos, yPos};
        } break;

        default: type = CommandInput{"null", 0ul}; break;
    }

    auto val = std::optional<PlayerInputAction>{PlayerInputAction{
        actioninfo->timestamp(), actioninfo->playercode(), (uint32_t)actioninfo->tick(), type}};

    currentaction_++;

    delete[] data;
    return val;
}

InputReproducer::~InputReproducer()
{
    if (f_) {
        fclose(f_);
    }
}

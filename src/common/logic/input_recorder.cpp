#include <cinttypes>
#include <common/logic/input_recorder.hpp>

#include "common/logic/input_serialize_generated.h"

using namespace familyline::logic;

InputRecorder::InputRecorder(PlayerManager& pm) : pm_(pm)
{
    pm_.addListener(std::bind(&InputRecorder::addAction, this, std::placeholders::_1));

    auto pmap = pm_.getPlayerNames();
    for (auto [id, name] : pmap) {
        pinfo_.push_back(RecordPlayerInfo{(uint64_t)id, name});
    }
}

/**
 * Create the file.
 *
 * Return true if it could create, false if it could not
 */
bool InputRecorder::createFile(std::string_view path)
{
    if (f_) fclose(f_);
    flatbuffers::FlatBufferBuilder builder;

    f_ = fopen(path.data(), "wb");

    // Add a header and a player list
    const char* magic = "FREC";
    uint32_t version = 1;

    fwrite((void*)magic, 1, 4, f_);
    fwrite((void*)&version, sizeof(version), 1, f_);

    std::vector<flatbuffers::Offset<familyline::PlayerInfo>> playerlist;

    for (auto& pi : pinfo_) {
        auto piName  = builder.CreateString(pi.name);
        auto piColor = builder.CreateString("#ff0000");
        auto piFlat  = CreatePlayerInfo(builder, pi.id, piName, piColor);

        playerlist.push_back(piFlat);
    }

    auto playervec = builder.CreateVector(playerlist.data(), playerlist.size());
    builder.Finish(playervec);
    uint32_t psize = builder.GetSize();
    fwrite(&psize, sizeof(psize), 1, f_);
    fwrite(builder.GetBufferPointer(), builder.GetSize(), 1, f_);

    if (!f_) return false;

    return true;
}


/// This will allow us to use std::visit with multiple variants at once, a thing
/// that should be part of C++20.
/// There are three locations that I use this. I hope this gets suggested for C++23
/// or C++26
template <class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;


/**
 * You should pass this function as a callback to the player manager
 * `addListener` function
 */
bool InputRecorder::addAction(PlayerInputAction pia)
{
    if (f_) {
        flatbuffers::FlatBufferBuilder builder;

        familyline::InputType type_val;
        flatbuffers::Offset<void> type_data;

        std::visit(
            overload{
                [&](const CommandInput& a) {
                    auto cstr = builder.CreateString(a.commandName);

                    std::array<unsigned long, 5> params;
                    if (auto* objectID = std::get_if<object_id_t>(&a.param); objectID) {
                        params = {*objectID};
                    } else if (auto* arr = std::get_if<std::array<int, 2>>(&a.param); arr) {
                        params = {(unsigned long)(*arr)[0], (unsigned long)(*arr)[1]};
                    }

                    auto pserialize = builder.CreateVector(params.data(), params.size());
                    auto cargs      = CreateCommandInputArgs(builder, pserialize);

                    auto cval = CreateCommandInput(builder, cstr, cargs);

                    type_val  = InputType_cmd;
                    type_data = cval.Union();
                },
                [&](const SelectAction& a) {
                    auto ovec  = builder.CreateVector(a.objects.data(), a.objects.size());
                    auto oobjs = CreateSelectActionObjects(builder, ovec);
                    auto cval  = CreateSelectAction(builder, oobjs);

                    type_val  = InputType_sel;
                    type_data = cval.Union();
                },
                [&](const ObjectMove& a) {
                    auto cval = CreateObjectMove(builder, a.xPos, a.yPos);

                    type_val  = InputType_obj_move;
                    type_data = cval.Union();
                },
                [&](const CameraMove& a) {
                    auto cval = CreateCameraMove(builder, a.deltaX, a.deltaY, a.deltaZoom);

                    type_val  = InputType_cam_move;
                    type_data = cval.Union();
                },
                [&](const CameraRotate& a) {
                    auto cval = CreateCameraRotate(builder, a.radians);

                    type_val  = InputType_cam_rotate;
                    type_data = cval.Union();
                },
                [&](const CreateEntity& a) {
                    auto strtype = builder.CreateString(a.type);
                    auto cval    = CreateCreateEntity(builder, strtype, a.xPos, a.yPos);

                    type_val  = InputType_create;
                    type_data = cval.Union();
                },
                [&](const AddSelectAction& a) {

                },
                [&](const CreateSelectGroup& a) {}, [&](const SelectGroup& a) {},
                [&](const AddSelectGroup& a) {}, [&](const RemoveSelectGroup& a) {}},
            pia.type);

        auto inputel = CreateInputElement(
            builder, pia.tick, pia.playercode, pia.timestamp, type_val, type_data);

        builder.Finish(inputel);

        uint32_t isize   = builder.GetSize();
        const char* imagic = "FINP";
        fwrite((void*)imagic, 1, 4, f_);
        fwrite(&isize, sizeof(isize), 1, f_);
        fwrite(builder.GetBufferPointer(), builder.GetSize(), 1, f_);

        inputcount_++;
    }

    return (f_ != nullptr);
}

void InputRecorder::commit()
{
    std::vector<flatbuffers::Offset<familyline::InputElement>> inputs;

    const char* endmagic = "FEND";
    uint32_t inputcount = inputcount_;
    uint32_t checksum = 0;
    
    fwrite((void*)endmagic, 1, 4, f_);
    fwrite(&inputcount, sizeof(inputcount), 1, f_);
    fwrite(&checksum, sizeof(checksum), 1, f_);
}

InputRecorder::~InputRecorder()
{
    if (f_) {
        this->commit();
        fclose(f_);
    }
}

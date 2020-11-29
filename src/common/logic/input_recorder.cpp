#include <cinttypes>
#include <common/logic/input_recorder.hpp>

#include "common/logic/input_serialize_generated.h"

using namespace familyline::logic;

InputRecorder::InputRecorder(PlayerManager& pm) : pm_(pm), builder_(1024)
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

    f_ = fopen(path.data(), "wb");
    if (!f_) return false;

    return true;
}

/**
 * You should pass this function as a callback to the player manager
 * `addListener` function
 */
bool InputRecorder::addAction(PlayerInputAction a)
{
    if (f_) {
        pia_list_.push_back(a);
    }

    return (f_ != nullptr);
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

void InputRecorder::commit()
{
    std::vector<flatbuffers::Offset<familyline::PlayerInfo>> playerlist;
    fseek(f_, 0, SEEK_SET);

    for (auto& pi : pinfo_) {
        auto piName  = builder_.CreateString(pi.name);
        auto piColor = builder_.CreateString("#ff0000");
        auto piFlat  = CreatePlayerInfo(builder_, pi.id, piName, piColor);

        playerlist.push_back(piFlat);
    }

    std::vector<flatbuffers::Offset<familyline::InputElement>> inputs;

    for (auto& pia : pia_list_) {
        familyline::InputType type_val;
        flatbuffers::Offset<void> type_data;

        std::visit(
            overload{
                [&](const CommandInput& a) {
                    auto cstr = builder_.CreateString(a.commandName);

                    std::array<unsigned long, 5> params;
                    if (auto* objectID = std::get_if<object_id_t>(&a.param); objectID) {
                        params = {*objectID};
                    } else if (auto* arr = std::get_if<std::array<int, 2>>(&a.param); arr) {
                        params = {(unsigned long)(*arr)[0], (unsigned long)(*arr)[1]};
                    }

                    auto pserialize = builder_.CreateVector(params.data(), params.size());
                    auto cargs = CreateCommandInputArgs(builder_, pserialize);
                    
                    auto cval       = CreateCommandInput(builder_, cstr, cargs);

                    type_val = InputType_cmd;
                    type_data = cval.Union();                    
                },
                [&](const SelectAction& a) {                    
                    auto ovec = builder_.CreateVector(a.objects.data(), a.objects.size());
                    auto oobjs = CreateSelectActionObjects(builder_, ovec);
                    auto cval = CreateSelectAction(builder_, oobjs);

                    type_val = InputType_sel;
                    type_data = cval.Union();
                },
                [&](const ObjectMove& a) {
                    auto cval = CreateObjectMove(builder_, a.xPos, a.yPos);
                    
                    type_val = InputType_obj_move;
                    type_data = cval.Union();
                },
                [&](const CameraMove& a) {
                    auto cval = CreateCameraMove(builder_, a.deltaX, a.deltaY, a.deltaZoom);
                    
                    type_val = InputType_cam_move;
                    type_data = cval.Union();
                    
                },
                [&](const CameraRotate& a) {
                    auto cval = CreateCameraRotate(builder_, a.radians);
                    
                    type_val = InputType_cam_rotate;
                    type_data = cval.Union();
                    
                },
                [&](const CreateEntity& a) {
                    auto strtype = builder_.CreateString(a.type);
                    auto cval = CreateCreateEntity(builder_, strtype, a.xPos, a.yPos);
                    
                    type_val = InputType_create;
                    type_data = cval.Union();                    
                },
                [&](const AddSelectAction& a) {
                    
                },
                [&](const CreateSelectGroup& a) {}, [&](const SelectGroup& a) {},
                [&](const AddSelectGroup& a) {}, [&](const RemoveSelectGroup& a) {}},
            pia.type);

        inputs.push_back(CreateInputElement(builder_, pia.tick, pia.playercode,
                                            pia.timestamp, type_val, type_data));
    }

    auto playervec = builder_.CreateVector(playerlist.data(), playerlist.size());
    auto inputvec = builder_.CreateVector(inputs.data(), inputs.size());
    InputFileBuilder ifb(builder_);
    ifb.add_player_info(playervec);
    ifb.add_input_element(inputvec);
    auto fileinfo = ifb.Finish();

    builder_.Finish(fileinfo, "FREC");
    
    fwrite(builder_.GetBufferPointer(), builder_.GetSize(), 1, f_);
}

InputRecorder::~InputRecorder()
{
    if (f_) {
        this->commit();
        fclose(f_);
    }
}

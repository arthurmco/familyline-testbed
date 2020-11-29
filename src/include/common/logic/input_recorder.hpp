#pragma once

/**
 * Records all received inputs from all clients into a single file, so
 * you can reproduce it later
 *
 * (C) 2020 Arthur Mendes
 */

#include <common/logic/player_actions.hpp>
#include <common/logic/player_manager.hpp>
#include <common/logic/input_serialize_generated.h>

namespace familyline::logic {

    struct RecordPlayerInfo {
        uint64_t id;
        std::string name;
    };

    class InputRecorder {
    private:
        FILE* f_ = nullptr;
        PlayerManager& pm_;
        std::vector<RecordPlayerInfo> pinfo_;

        flatbuffers::FlatBufferBuilder builder_;
        std::vector<PlayerInputAction> pia_list_;
        
    public:
        InputRecorder(PlayerManager& pm);

        /**
         * Create the file.
         *
         * Return true if it could create, false if it could not
         */
        bool createFile(std::string_view path);


        /**
         * You should pass this function as a callback to the player manager
         * `addListener` function
         */
        bool addAction(PlayerInputAction a);

        void commit();

        ~InputRecorder();

        InputRecorder(InputRecorder&) = delete;
        InputRecorder(const InputRecorder&) = delete;

    };


}

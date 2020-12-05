#pragma once

/**
 * Reproduces a previously recorded input
 *
 * (C) 2020 Arthur Mendes
 */

#include <string>
#include <vector>
#include <tuple>

#include <common/logger.hpp>
#include <common/logic/player_session.hpp>
#include <common/logic/player_actions.hpp>
#include <cstdint>
#include <input_serialize_generated.h>

namespace familyline::logic {

    struct InputInfo {
        std::string name;
        std::string color;
        uint64_t id;

        std::vector<uint64_t> allies;
        std::vector<uint64_t> enemies;
    };

    class ReplayPlayer;

    class InputReproducer {
    public:
        InputReproducer(std::string_view file)
            : file_(file), f_(nullptr), pinfo_({}), actioncount_(0), currentaction_(0)
            {}

        
        /**
         * Open and verify the file
         *
         * It will return true on success, and false on error
         */
        bool open();

        /**
         * Create a player session with players that will
         * reproduce what happened in the file
         *
         * The diplomacy will be kept neutral between everyone, but the
         * correct thing to do is to save it into the file
         */
        PlayerSession createPlayerSession(Terrain& terrain);

        InputReproducer(InputReproducer&) = delete;
        InputReproducer(const InputReproducer&) = delete;

        std::string getTerrainFile() { return "terrain_test.flte"; }

        /**
         * Get the next action from the file
         *
         * If no more actions exist, returns an empty optional
         */
        std::optional<PlayerInputAction> getNextAction();

        uint64_t getCurrentActionIndex() const { return currentaction_; }

        void reset() {
            if (f_) {
                fseek(f_, off_actionlist_, SEEK_SET);
                currentaction_ = 0;
            }
        }

        bool isReproductionEnded() const;

        /**
         * Dispatch events to the players, from nextTick_ to nextTick_+nextTicks
         */
        void dispatchEvents(unsigned nextTicks);

        ~InputReproducer();

    private:
        std::string file_;
        FILE* f_;
        std::vector<InputInfo> pinfo_;

        off_t off_actionlist_ = 0;
        long long int actioncount_, currentaction_;
        
        void onActionEnd(ReplayPlayer* p);
        
        std::map<int,
                 std::function<void(PlayerInputAction)>> action_callbacks_;
        std::map<int, bool> player_ended_;

        int nextTick_ = 0;
        std::optional<PlayerInputAction> last_action_;


        void dispatchAction(const PlayerInputAction& action);
   };

}

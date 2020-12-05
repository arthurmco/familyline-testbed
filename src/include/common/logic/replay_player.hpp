#pragma once

/**
 * A player that reproduces input that comes from the
 * InputReproducer
 *
 * (C) 2020 Arthur Mendes
 */

#include <queue>

#include <common/logic/input_reproducer.hpp>
#include <common/logic/player.hpp>

#include "common/logic/player_actions.hpp"

namespace familyline::logic
{
class ReplayPlayer : public Player
{
private:
    InputReproducer& ir_;

    std::queue<PlayerInputAction> actions_;

public:
    ReplayPlayer(
        PlayerManager& pm, const Terrain& terr, const char* name, int code, InputReproducer& ir)
        : Player(pm, terr, name, code), ir_(ir)
    {
    }

    /**
     * Push an action
     *
     * You will need to push an action in tick order
     */
    void enqueueAction(PlayerInputAction a);

    /**
     * Generate the input actions.
     *
     * They must be pushed to the input manager
     */
    virtual void generateInput();
};

}  // namespace familyline::logic

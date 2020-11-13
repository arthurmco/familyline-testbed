#pragma once

/**
 * Player enumeration routines
 *
 * To start a game, first we need to tell the player managers about
 * what players we have in our system
 *
 * Copyright (C) 2020 Arthur Mendes
 */

#include <common/logic/colony_manager.hpp>
#include <common/logic/player_manager.hpp>
#include <memory>
#include <tuple>

namespace familyline
{
/**
 * Player information
 *
 * Currently, only the name.
 * Not even the color will appear
 */
struct InitPlayerInfo {
    std::string name;
    int id = -1;  /// used only for storing the ID from the player manager
};

struct PlayerSession {
    std::unique_ptr<logic::PlayerManager> players;
    std::unique_ptr<logic::ColonyManager> colonies;

    std::map<unsigned int /*player_id*/, std::reference_wrapper<logic::Colony>> player_colony;
};

/**
 * Create a single player player manager.
 *
 * In the future, this will add the AI code to the player manager,
 * but now, only the dummy player (see the cpp file) and the
 * human player will be added.
 */
PlayerSession initSinglePlayerSession(logic::Terrain& terrain, InitPlayerInfo& human_info);

}  // namespace familyline

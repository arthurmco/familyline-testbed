/**
 * Represents a colony, your city in-game
 *
 * Copyright (C) 2020 Arthur Mendes
 */

#pragma once

#include <common/logic/player.hpp>
#include <common/logic/alliance.hpp>
#include <string_view>

namespace familyline::logic {

    /*
     * The colony is the owner of the game entities
     *
     * It is the colony that defines colors and diplomacies
     */
    class Colony {
    private:
        Player& player_;

        /**
         * The color of the buildings
         */
        unsigned color_;

        /**
         * The colony's alliance.
         *
         * This is what defines the diplomacy
         */
        Alliance& alliance_;

    public:
        Colony(Player& p, unsigned color, Alliance& a);
        
        Alliance& getAlliance() const { return alliance_; }

        std::string_view getName() const { return player_.getName(); }
    };

}

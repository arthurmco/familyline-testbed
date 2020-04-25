/**
 * Manages all the colonies in game
 *
 * See \Colony
 *
 * Copyright (C) 2020 Arthur Mendes
 */

#pragma once

#include <optional>
#include <vector>
#include <memory>
#include <common/logic/colony.hpp>
#include <common/logic/alliance.hpp>

namespace familyline::logic {
    
    /**
     * The colony manager
     *
     * Manages colonies and alliances.
     * You will need to create a colony from here, so the alliances are
     * automatically created, because you need alliances to set diplomacy
     *
     * Also, since we return a reference of alliance to the players, it is good 
     * not to delete any alliance that is at the middle of two valid alliances, so
     * the references will not be misaligned.
     * Also, we reserve a lot of space, because, if the vector is resized due
     * to lack of current space, the references will be lost.
     */
    class ColonyManager {
    private:
        std::vector<std::unique_ptr<Colony>> colonies_;
        std::vector<Alliance> alliances_;

    public:
        const static int MaxColonies = 128;
    
        ColonyManager();
        
        Colony& createColony(Player& p, unsigned color,
                             std::optional<std::reference_wrapper<Alliance>> alliance);
        
        Alliance& createAlliance(std::string name);
        
        /**
         * Get diplomacy between colonies.
         */
        DiplomacyStatus getDiplomacy(Colony const& c, Colony const& other) const;
        
        /**
         * Set diplomacy of the alliance `a` to `other`, how `a` views `other`
         *
         * This does not affect how `other` views `a`.
         */
        void setAllianceDiplomacy(Alliance& a, Alliance& other, DiplomacyStatus s);    
    };


}

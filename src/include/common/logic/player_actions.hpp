#pragma once

/**
 * Control player input actions
 *
 * Since they depend on the player, not on the input, and the player info
 * lives on the logic module, they will go here, and not on the input module
 * 
 * (C) 2020 Arthur M.
 */

#include <cstdint>
#include <string>
#include <variant>

namespace familyline::logic {

    
    /**
     * Build some object
     */
    struct BuildAction {
        std::string type_name;
    };

    /**
     * Select an object
     */
    struct ObjectSelectAction {
        long int objectID;
    };


    /**
     * Move the selected object to some location
     */
    struct ObjectMoveAction {
        double destX, destZ;
    };

    /**
     * Make the selected object do the 'use' action in some other object.
     * This action varies from object to object. It can be harvest, attack
     * or trade
     *
     * It is the action you run when you right-click.
     */
    struct ObjectUseAction {
        long int useWhat;
    };

    /**
     * Make the selected object run the specified action
     *
     * The action is a string, it can be anything, but the selected object
     * should understand
     */
    struct ObjectRunAction {
        std::string actionName;
    };
    
    /**
     * Controls camera positioning
     *
     * Might be useful on recorded games, to see and study where the player
     * focus on.
     */
    struct CameraMove {
        double deltaX, deltaY;
        double deltaZoom;
    };

    using PlayerInputType =
        std::variant<BuildAction, ObjectSelectAction, ObjectMoveAction,
                     ObjectUseAction, ObjectRunAction, CameraMove>;

    
    struct PlayerInputAction {
        uint64_t timestamp;
        uint64_t playercode;
        uint32_t tick;

        PlayerInputType type;
    };

}

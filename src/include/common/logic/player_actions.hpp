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
     * Enqueue the build of some object
     */
    struct EnqueueBuildAction {
        std::string type_name;
    };

    /**
     * Commit the last build, starts building it at the specified game
     * position.
     * The server should verify that the build position is valid, that is
     * it is inside the camera vision range
     *
     * last_build value means, if false, that we should expect more
     * commits 
     */
    struct CommitLastBuildAction {
        double destX, destZ;
        double destY; // do not serialize this value, it can be determined automatically
        bool last_build;
    };
    
    /**
     * Select an object
     */
    struct ObjectSelectAction {
        long int objectID;
    };

    /**
     * Clear object selection
     */
    struct SelectionClearAction {
        int dummy = 0;
    };


    /**
     * Move the selected object to some location
     */
    struct SelectedObjectMoveAction {
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
        std::variant<EnqueueBuildAction, CommitLastBuildAction,
                     ObjectSelectAction, SelectedObjectMoveAction, SelectionClearAction,
                     ObjectUseAction, ObjectRunAction, CameraMove>;

    
    struct PlayerInputAction {
        uint64_t timestamp;
        uint64_t playercode;
        uint32_t tick;

        PlayerInputType type;
    };

}

#pragma once

// Decorations = location
// Unit        = location attack            movement
// Building    = location attack container?
// Transporter = location attack container  movement

#include <vector>
#include <optional>
#include "imesh.hpp"
#include "terrain.hpp"

namespace familyline::logic {
    class GameObject;

// TODO: Handle attacks on its component or elsewhere?

/**
 * Allows objects to have something visible, something you can see on 
 * screen that represents it.
 */
    struct LocationComponent {
        IMesh* mesh;
        GameObject* object;

        /**
         * Updates a mesh by getting a coordinate in the logic space, converting it to
         * the graphical/OpenGL space and then setting it as the mesh position
         */
        void updateMesh(const Terrain& t, glm::vec3);
    };

/**
 * Allows objects to attack and be attacked.
 */
    struct AttackComponent {
        GameObject* object;

        /**
         * atkRanged is the power of ranged attack. defRanged is defense against those attacks
         * atkMelee is the power of melee attack. defMelee is defense against it
         * atkSiege is the attack against buildings, since siege weapons are made to destroy them
         * atkTransporter is the attack against transporter units. Mostly ships
         *
         * Probably you know what defSiege and defTransporter means.
         *
         * atkRanged and atkMelee does not have effects on buildings
         */

        double atkRanged, atkMelee, atkSiege, atkTransporter;
        double defRanged, defMelee, defSiege, defTransporter;

        /**
         * Unit rotation, in radians
         */
        double rotation;

        /**
         * Attack maximum distance, in game units. 
         *
         * If the attacked unit is more distant than this value, it cannot be harmed
         * by the unit that started the attack.
         */
        double atkDistance;

        /**
         * Unit armor
         *
         * Will protect from attacks. Attacks that have the score lower than the
         * armor will damage both the armor and the health equally instead of the 
         * unit health, until the armor is destroyed. Then, it will damage the unit
         * directly
         * 
         * The armor will replenish itself when the unit is not attacking or being 
         * attacked.
         */
        double armor = 0;

        /**
         * Unit range, in radians
         *
         * The range is symmetrical, the zero axis is the unit "normal", where it is
         * looking at, and it is mirrored against this vector. So a range of 45deg is
         * actually 90deg in total, 45 each side of the unit.
         */
        double range;

        /**
         * Attack directly another object
         *
         * Do a certain amount of damage to him.
         * Return the pure damage dealt, or an empty optional if the target is out of range
         */
        std::optional<double> doDirectAttack(const AttackComponent& other);
    };

/**
 * Allows objects to have objects inside them
 *
 * The objects inside the building might or might not affect the properties
 * of the container object, the engine have no restrictions against it
 *
 * We also will not impose restrictions of what you can store, but please
 * try not to store buildings inside it
 */
    struct ContainerComponent {
        int maxObjects;
        std::vector<GameObject*> storedObjects;
    };

/**
 * Allows objects to move
 */
    struct MovementComponent {

        //! Tenths of game units per second
        double speed;
    };
}

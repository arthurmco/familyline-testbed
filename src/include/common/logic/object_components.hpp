#pragma once

// Decorations = location
// Unit        = location attack            movement
// Building    = location attack container?
// Transporter = location attack container  movement

#include <common/logic/terrain.hpp>
#include <common/logic/imesh.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <vector>
#include <tl/expected.hpp>
#include <variant>

namespace familyline::logic
{


/**
 * The attack type
 */
enum class AttackType { Melee, Projectile, Other };

struct AttackTypeMelee {
    /// Maximum distance a melee attack can be done
    double a = 0;
};

struct AttackTypeProjectile {
    /// Projectile speed, in units/tick
    double projectileSpeed;
};

using AttackTypeClass = std::variant<AttackTypeMelee, AttackTypeProjectile, std::monostate>;

/**
 * The attack rule
 *
 * We have, for now, two attack types: melee and projectile attacks.
 * Each one is governed by a certain rule, who dictates that what attack will
 * be applied, depending on the distance between two entities.
 */
struct AttackRule {
    /// Minimum an maximum distance this attack type can be done
    double minDistance, maxDistance;
    AttackTypeClass ctype;
};

/**
 * Information about an attack
 */
struct AttackData {
    /// The type of the attack
    AttackType atype;

    /// The chosen rule for this attack
    AttackRule rule;

    /// The damage value it inflicted
    double value;
};

/**
 * An error explaining why a certain attack could not be done
 */
enum class AttackError { DefenderTooNear, DefenderTooFar, DefenderNotInSight };

/**
 * The attack attributes
 */
struct AttackAttributes {

    /// How much health points it will remove from the attacked unit
    double attackPoints;

    /// How much points it will decrease from the attack of the attacking unit?
    double defensePoints;

    /// The speed of our attack.
    ///
    /// It is use more or less like this:
    /// We get the number 2048 and divide by the attack speed to get an
    /// attack interval (`ai`)
    /// When you do an attack, even if the precision calculations does not
    /// make it succeed, we set a counter to `ai`, and reduce it on each
    /// tick.
    /// When the attack interval reaches zero, we try to attack again.
    /// If the attack interval is less than one, we attack multiple times
    /// until it becomes one.
    /// `ai` cannot be bigger than 128, so the attack speed cannot be
    /// bigger than 262144
    ///
    /// Note that an attack speed of 2048 means that you will attack
    /// once per tick.
    double attackSpeed;

    /// The chance of your attack causing damage to the enemy
    /// 100% is always, 0% is never.
    double precision;

    /// Maximum angle that the defender you have to be from you so you can
    /// hit it, assuming it is into the correct distance
    double maxAngle;
};


    class GameObject;
    
/**
 * The attack component for our entity
 *
 * You will note that only basic attack decision (aka if you will attack or not)
 * is kept here, most of the real attack code resides on the AttackManager.
 */
class AttackComponent
{
public:
    AttackComponent(AttackAttributes base_attribs, std::vector<AttackRule> rules)
        : attributes_(base_attribs), rules_(rules)
    {
    }

    tl::expected<AttackData, AttackError> attack(const AttackComponent& other);

    const AttackAttributes& attributes() const { return this->attributes_; }
    
    /**
     * Calculates the base damage that would be inflicted if the attack was done
     * right now, but using only the component attributes.
     *
     * This method is designed to be used for the ones that want to calculate
     * the damage based on the events
     */
    static double calculateDamage(
        const AttackAttributes& attacker, const AttackAttributes& defender);

    /**
     * Returns true if you can attack, false if you cannot
     */
    bool isInRange(const AttackComponent& other) const;

    void setParent(GameObject* parent) { parent_ = parent; }

private:
    GameObject* parent_ = nullptr;

    /**
     * Gets the distance in game units, and angle in radians, between two entities
     *
     * About the angle:
     *   If it is directly in front of you, for example, this should return 0.
     *   If it is directly behind you, this should return PI (180deg in radians)
     *
     * The first element of the tuple is the distance, the second is the angle.
     */
    std::tuple<double, double> calculateDistanceAndAngle(const AttackComponent& other) const;

    std::optional<AttackError> checkIfInRange(
        const AttackRule& currentrule, std::tuple<double, double> distanceAndAngle) const;

    /**
     * Calculate the base damage that would be inflicted if the attack was now
     */
    double damage(const AttackComponent& other) const;

    /**
     * Reescale the position of 'base', assuming the position of 'obj' is 0,0
     */
    glm::vec3 assumeObjectIsCenter(const GameObject& obj, glm::vec3 base) const;

    /**
     * Convert a certain position from cartesian to polar coordinates
     *
     * This is done so we can determine if we are inside the angle of attack
     * In the returned vec2, x is the distance and y is the angle.
     */
    glm::vec2 cartesianToPolar(double x, double y) const;

    /**
     * Send the event to the current action queue
     */
    void sendEvent(
        const AttackComponent& other, const AttackRule& rule,
        const AttackAttributes& atkAttributes, const AttackAttributes& defAttributes);

    AttackAttributes attributes_;
    std::vector<AttackRule> rules_;
};


/**
 * Allows objects to have something visible, something you can see on
 * screen that represents it.
 */
struct LocationComponent {
    std::shared_ptr<IMesh> mesh;
    GameObject* object;

    /**
     * Updates a mesh by getting a coordinate in the logic space, converting it to
     * the graphical/OpenGL space and then setting it as the mesh position
     */
    void updateMesh(const Terrain& t);
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

class Colony;

/**
 * Binds a colony to an object
 *
 * Objects without this component will not be bound to any colony.
 * Objects with this component but with no colony will be bound
 * to "Gaia", the "nature" city
 *
 * You should always define this component, unless the game object
 * is something from nature, like a tree or a resource extraction
 * entity, or some decorative-only object.
 */
struct ColonyComponent {
    std::optional<std::reference_wrapper<Colony>> owner;

    ColonyComponent();
};

}  // namespace familyline::logic

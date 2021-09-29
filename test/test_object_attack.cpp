#include <gtest/gtest.h>

#include <cmath>
#include <common/logic/attack_manager.hpp>
#include <common/logic/lifecycle_manager.hpp>
#include <common/logic/logic_service.hpp>
#include <glm/fwd.hpp>
#include <initializer_list>
#include <optional>
#include <tl/expected.hpp>
#include <variant>
#include <vector>

#include "common/logic/game_object.hpp"
#include "common/logic/input_reproducer.hpp"
#include "utils.hpp"

using namespace familyline::logic;

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
struct _AttackData {
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
    double attackSpeed;

    /// The chance of your attack causing damage to the enemy
    double precision;

    /// Maximum angle that the defender you have to be from you so you can
    /// hit it, assuming it is into the correct distance
    double maxAngle;
};

/**
 * The attack component for our entity
 *
 * You will note that only basic attack decision (aka if you will attack or not)
 * is kept here, most of the real attack code resides on the AttackManager.
 */
class _AttackComponent
{
public:
    _AttackComponent(AttackAttributes base_attribs, std::vector<AttackRule> rules)
        : attributes_(base_attribs), rules_(rules)
    {
    }

    tl::expected<_AttackData, AttackError> attack(const _AttackComponent& other);

    /**
     * Returns true if you can attack, false if you cannot
     */
    bool isInRange(const _AttackComponent& other) const;

    void setParent(GameObject* parent) { parent_ = parent; }

private:
    GameObject* parent_;

    /**
     * Gets the distance in game units, and angle in radians, between two entities
     *
     * About the angle:
     *   If it is directly in front of you, for example, this should return 0.
     *   If it is directly behind you, this should return PI (180deg in radians)
     *
     * The first element of the tuple is the distance, the second is the angle.
     */
    std::tuple<double, double> calculateDistanceAndAngle(const _AttackComponent& other) const;

    std::optional<AttackError> checkIfInRange(
        const AttackRule& currentrule, std::tuple<double, double> distanceAndAngle) const;

    /**
     * Calculate the base damage that would be inflicted if the attack was now
     */
    double damage(const _AttackComponent& other) const;

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
    void sendEvent(const _AttackComponent& other);

    AttackAttributes attributes_;
    std::vector<AttackRule> rules_;
};

static uint64_t nextAttackID = 1;

/**
 * Send the event to the current action queue
 */
void _AttackComponent::sendEvent(const _AttackComponent& other)
{
    auto atkpos = this->parent_->getPosition();
    auto defpos = other.parent_->getPosition();

    LogicService::getActionQueue()->pushEvent(EntityEvent{
        0,
        EventAttackStart{
            .attackerID = this->parent_->getID(),
            .defenderID = other.parent_->getID(),
            .attackID   = nextAttackID++,
            .atkXPos    = (unsigned int)atkpos.x,
            .atkYPos    = (unsigned int)atkpos.z,
            .defXPos    = (unsigned int)defpos.x,
            .defYPos    = (unsigned int)defpos.z,
        },
        nullptr});
}

tl::expected<_AttackData, AttackError> _AttackComponent::attack(const _AttackComponent& other)
{
    auto distAndAngle = this->calculateDistanceAndAngle(other);
    auto rule         = rules_[0];
    auto not_in_range = this->checkIfInRange(rule, distAndAngle);
    if (not_in_range) return tl::make_unexpected(*not_in_range);

    this->sendEvent(other);
    return _AttackData{
        .atype = AttackType::Melee, .rule = rule, .value = glm::max(this->damage(other), 0.01)};
}

/**
 * Calculate the base damage that would be inflicted if the attack was now
 */
double _AttackComponent::damage(const _AttackComponent& other) const
{
    return this->attributes_.attackPoints - other.attributes_.defensePoints;
}

bool _AttackComponent::isInRange(const _AttackComponent& other) const
{
    auto distAndAngle = this->calculateDistanceAndAngle(other);
    return !this->checkIfInRange(rules_[0], distAndAngle).has_value();
}

/**
 * Reescale the position of 'base', assuming the position of 'obj' is 0,0
 */
glm::vec3 _AttackComponent::assumeObjectIsCenter(const GameObject& obj, glm::vec3 base) const
{
    auto pos = obj.getPosition();
    return base - pos;
}

/**
 * Convert a certain position from cartesian to polar coordinates
 *
 * This is done so we can determine if we are inside the angle of attack
 * In the returned vec2, x is the distance and y is the angle in radians.
 */
glm::vec2 _AttackComponent::cartesianToPolar(double x, double y) const
{
    // x = r*cos O
    // y - r*sen O
    auto radius = sqrt(x * x + y * y);
    auto angle  = acos(x / radius);

    return glm::vec2(radius, angle);
}

/**
 * Gets the distance in game units, and angle in radians, between two entities
 *
 * About the angle:
 *   If it is directly in front of you, for example, this should return 0.
 *   If it is directly behind you, this should return PI (180deg in radians)
 *
 * The first element of the tuple is the distance, the second is the angle.
 */
std::tuple<double, double> _AttackComponent::calculateDistanceAndAngle(
    const _AttackComponent& other) const
{
    auto defPos   = this->assumeObjectIsCenter(*this->parent_, other.parent_->getPosition());
    auto defPolar = this->cartesianToPolar(defPos.x, defPos.z);

    return std::make_tuple(double(defPolar.x), double(defPolar.y));
}

std::optional<AttackError> _AttackComponent::checkIfInRange(
    const AttackRule& currentrule, std::tuple<double, double> distanceAndAngle) const
{
    auto [distance, angle] = distanceAndAngle;

    auto maxangle = this->attributes_.maxAngle;
    auto fixangle = angle - M_PI;

    if (distance < currentrule.minDistance) return std::make_optional(AttackError::DefenderTooNear);

    if (distance > currentrule.maxDistance) return std::make_optional(AttackError::DefenderTooFar);

    if (fixangle < -maxangle / 2 || fixangle > maxangle / 2) {
        return std::make_optional(AttackError::DefenderNotInSight);
    }

    return std::nullopt;
}

/****************/

/**
 * The attack manager
 *
 * Causes the real damage, calculate  the damage of the projectiles, etc.
 */
class _AttackManager
{
public:
    /**
     * Do the damage, update the projectile positions and other multiple things
     *
     * Runs once per tick
     */
    void update();

private:
};

TEST(AttackManager, BaseMeleeAttack)
{
    _AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 32,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    _AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 32,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    auto atker =
        make_object({"atker", "Attacker", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});
    auto defer =
        make_object({"defder", "Defender", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});

    atker->setPosition(glm::vec3(4, 0, 4));
    defer->setPosition(glm::vec3(1, 0, 1));

    atk.setParent(atker.get());
    def.setParent(defer.get());

    ASSERT_TRUE(atk.isInRange(def));
    auto attack = atk.attack(def);
    ASSERT_TRUE(attack.has_value());
    ASSERT_EQ(AttackType::Melee, attack->atype);
    ASSERT_EQ(0.25, attack->value);
}

TEST(AttackManager, TooFarMeleeAttack)
{
    _AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 32,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    _AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 32,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    auto atker =
        make_object({"atker", "Attacker", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});
    auto defer =
        make_object({"defder", "Defender", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});

    atker->setPosition(glm::vec3(6, 0, 6));
    defer->setPosition(glm::vec3(1, 0, 1));

    atk.setParent(atker.get());
    def.setParent(defer.get());

    ASSERT_FALSE(atk.isInRange(def));
    auto attack = atk.attack(def);
    ASSERT_FALSE(attack.has_value());
    ASSERT_EQ(AttackError::DefenderTooFar, attack.error());
}

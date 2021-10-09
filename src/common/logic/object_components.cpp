#include <algorithm>  //for std::max
#include <cmath>
#include <common/logic/terrain.hpp>
#include <common/logic/game_object.hpp>
#include <common/logic/object_components.hpp>
#include <common/logic/logic_service.hpp>
#include <common/logger.hpp>

using namespace familyline::logic;

#ifdef WIN32
#define M_PI 3.1415
#endif

/**
 * Updates a mesh by getting a coordinate in the logic space, converting it to
 * the graphical/OpenGL space and then setting it as the mesh position
 */
void LocationComponent::updateMesh(const Terrain& t)
{
    glm::vec3 pos = this->object->getPosition();

    auto height = t.getHeightFromCoords(glm::vec2(pos.x, pos.z));

    //	this->mesh->setPosition(t.renderer->convertToModelSpace(glm::vec3(pos.x, height, pos.y)));
    this->mesh->setLogicPosition(t.gameToGraphical(glm::vec3(pos.x, height, pos.z)));
}


static uint64_t nextAttackID = 1;

std::shared_ptr<EventEmitter> attack_emitter = std::shared_ptr<EventEmitter>();

/**
 * Send the event to the current action queue
 */
void AttackComponent::sendEvent(
    const AttackComponent& other, const AttackRule& rule, const AttackAttributes& atkAttributes,
    const AttackAttributes& defAttributes)
{
    auto atkpos = this->parent_->getPosition();
    auto defpos = other.parent_->getPosition();

    if (!attack_emitter) {
        attack_emitter = std::make_shared<EventEmitter>("attack-component-emitter");
        LogicService::getActionQueue()->addEmitter(attack_emitter.get());
    }

    auto e = EntityEvent{
        0,
        EventAttackStart{
            .attackerID    = this->parent_->getID(),
            .defenderID    = other.parent_->getID(),
            .attackID      = nextAttackID++,
            .rule          = rule,
            .atkAttributes = atkAttributes,
            .defAttributes = defAttributes,
            .atkXPos       = (unsigned int)atkpos.x,
            .atkYPos       = (unsigned int)atkpos.z,
            .defXPos       = (unsigned int)defpos.x,
            .defYPos       = (unsigned int)defpos.z,
        },
        nullptr};
    attack_emitter->pushEvent(e);
}

tl::expected<AttackData, AttackError> AttackComponent::attack(const AttackComponent& other)
{
    auto distAndAngle = this->calculateDistanceAndAngle(other);
    auto rule         = rules_[0];
    auto not_in_range = this->checkIfInRange(rule, distAndAngle);
    if (not_in_range) return tl::make_unexpected(*not_in_range);

    this->sendEvent(other, rule, this->attributes_, other.attributes_);

    auto &log = LoggerService::getLogger();

    auto dmg = this->damage(other);
    log->write(
        "attack-component", LogType::Debug,
        "'{}' ({}) attacking '{}' ({}) with damage of {}",
        this->parent_ ? this->parent_->getName() : "(null)",
        this->parent_ ? this->parent_->getID() : -1,
        other.parent_ ? other.parent_->getName() : "(null)",
        other.parent_ ? other.parent_->getID() : -1,
        dmg);
    
    return AttackData{
        .atype = AttackType::Melee, .rule = rule, .value = dmg};
}

/**
 * Calculate the base damage that would be inflicted if the attack was now
 */
double AttackComponent::damage(const AttackComponent& other) const
{
    return AttackComponent::calculateDamage(this->attributes_, other.attributes_);
}

/**
 * Calculates the base damage that would be inflicted if the attack was done
 * right now, but using only the component attributes.
 *
 * This method is designed to be used for the ones that want to calculate
 * the damage based on the events
 */
double AttackComponent::calculateDamage(
    const AttackAttributes& attacker, const AttackAttributes& defender)
{
    return glm::max(attacker.attackPoints - defender.defensePoints, 0.01);
}

bool AttackComponent::isInRange(const AttackComponent& other) const
{
    auto distAndAngle = this->calculateDistanceAndAngle(other);
    return !this->checkIfInRange(rules_[0], distAndAngle).has_value();
}

/**
 * Reescale the position of 'base', assuming the position of 'obj' is 0,0
 */
glm::vec3 AttackComponent::assumeObjectIsCenter(const GameObject& obj, glm::vec3 base) const
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
glm::vec2 AttackComponent::cartesianToPolar(double x, double y) const
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
std::tuple<double, double> AttackComponent::calculateDistanceAndAngle(
    const AttackComponent& other) const
{
    assert(this->parent_);
    auto defPos   = this->assumeObjectIsCenter(*this->parent_, other.parent_->getPosition());
    auto defPolar = this->cartesianToPolar(defPos.x, defPos.z);

    return std::make_tuple(double(defPolar.x), double(defPolar.y));
}

std::optional<AttackError> AttackComponent::checkIfInRange(
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


ColonyComponent::ColonyComponent() : owner(std::optional<std::reference_wrapper<Colony>>()) {}

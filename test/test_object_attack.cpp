#include <gtest/gtest.h>

#include <cmath>
#include <common/logic/attack_manager.hpp>
#include <common/logic/lifecycle_manager.hpp>
#include <common/logic/logic_service.hpp>
#include <cstdint>
#include <glm/fwd.hpp>
#include <initializer_list>
#include <optional>
#include <tl/expected.hpp>
#include <variant>
#include <vector>

#include "common/logic/game_event.hpp"
#include "common/logic/game_object.hpp"
#include "common/logic/input_reproducer.hpp"
#include "common/logic/object_components.hpp"
#include "common/logic/object_manager.hpp"
#include "utils.hpp"

using namespace familyline::logic;

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
    void sendEvent(
        const _AttackComponent& other, const AttackRule& rule,
        const AttackAttributes& atkAttributes, const AttackAttributes& defAttributes);

    AttackAttributes attributes_;
    std::vector<AttackRule> rules_;
};

static uint64_t nextAttackID = 1;

std::shared_ptr<EventEmitter> attack_emitter = std::shared_ptr<EventEmitter>();

/**
 * Send the event to the current action queue
 */
void _AttackComponent::sendEvent(
    const _AttackComponent& other, const AttackRule& rule, const AttackAttributes& atkAttributes,
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

tl::expected<_AttackData, AttackError> _AttackComponent::attack(const _AttackComponent& other)
{
    auto distAndAngle = this->calculateDistanceAndAngle(other);
    auto rule         = rules_[0];
    auto not_in_range = this->checkIfInRange(rule, distAndAngle);
    if (not_in_range) return tl::make_unexpected(*not_in_range);

    this->sendEvent(other, rule, this->attributes_, other.attributes_);

    return _AttackData{
        .atype = AttackType::Melee, .rule = rule, .value = glm::max(this->damage(other), 0.01)};
}

/**
 * Calculate the base damage that would be inflicted if the attack was now
 */
double _AttackComponent::damage(const _AttackComponent& other) const
{
    return _AttackComponent::calculateDamage(this->attributes_, other.attributes_);
}

/**
 * Calculates the base damage that would be inflicted if the attack was done
 * right now, but using only the component attributes.
 *
 * This method is designed to be used for the ones that want to calculate
 * the damage based on the events
 */
double _AttackComponent::calculateDamage(
    const AttackAttributes& attacker, const AttackAttributes& defender)
{
    return attacker.attackPoints - defender.defensePoints;
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
    _AttackManager();
    ~_AttackManager();

    /**
     * Do the damage, update the projectile positions and other multiple things
     *
     * Runs once per tick
     */
    void update(ObjectManager& om, ObjectLifecycleManager& olm);

private:
    /**
     * Information about an attack.
     * Will be used so we can replay the attack until the entities are too apart,
     * from each other, the player sends one unit elsewhere, or one of them is dead.
     */
    struct AttackInfo {
        entity_id_t attackerID;
        entity_id_t defenderID;

        uint64_t attackID;

        AttackRule rule;
        AttackAttributes atkAttributes;
        AttackAttributes defAttributes;

        glm::vec2 atkPosition;
        glm::vec2 defPosition;

        /// The amount of successful attacks, the ones
        /// that did hit the target
        int successful_attacks;

        /// The amount of total attacks done.
        int total_attacks;

        /// Amount of ticks to the next attack
        /// It is double because, for hyper fast entities, we can have
        /// increments between 0 and 1, and so we have two attacks in the same
        /// tick.
        double ticks_until_attack;
    };

    /**
     * Based on the specified attack precision, check if the
     * next attack will happen
     *
     * TODO: use a global pseudo-RNG. Also make every game have a seed.
     */
    bool isNextAttackPrecise(unsigned precision) const;

    /**
     * Damage the game object.
     *
     * Return its remaining health points
     */
    double damageObject(GameObject& o, double damage);

    void sendAttackDoneEvent(
        const AttackInfo& atk, AttackAttributes atkAttribute, double damageDealt);
    void sendAttackMissEvent(const AttackInfo& atk);
    void sendDyingEvent(const AttackInfo& atk);

    void updateAttackInfo(const GameObject& attacker, const GameObject& defender, AttackInfo& atk);

    double getAttackInterval(const AttackAttributes& attr) const { return 2048 / attr.attackSpeed; }

    /**
     * Start a new countdown to the next attack, by incrementing
     * the ticks_until_attack by the attack interval.
     */
    void countUntilNextAttack(AttackInfo& atk);

    std::vector<AttackInfo> attacks_;
    EventEmitter emitter_ = EventEmitter("attack-manager-emitter");
    bool receiveAttackEvents(const EntityEvent& e);
};

_AttackManager::_AttackManager()
{
    using namespace std::placeholders;

    LogicService::getActionQueue()->addReceiver(
        "attack-manager", std::bind(&_AttackManager::receiveAttackEvents, this, _1),
        {ActionQueueEvent::AttackStart});

    LogicService::getActionQueue()->addEmitter(&emitter_);
}

_AttackManager::~_AttackManager()
{
    LogicService::getActionQueue()->removeReceiver("attack-manager");
    LogicService::getActionQueue()->removeEmitter(&emitter_);
}

/**
 * Damage the game object.
 *
 * Return its remaining health points
 */
double _AttackManager::damageObject(GameObject& o, double damage) { return o.addHealth(-damage); }

void _AttackManager::sendAttackDoneEvent(
    const AttackInfo& atk, AttackAttributes atkAttribute, double damageDealt)
{
    auto atkpos = atk.atkPosition;
    auto defpos = atk.defPosition;

    EntityEvent ev{
        0,
        EventAttackDone{
            .attackerID   = atk.attackerID,
            .defenderID   = atk.defenderID,
            .attackID     = atk.attackID,
            .atkXPos      = (unsigned)atkpos.x,
            .atkYPos      = (unsigned)atkpos.y,
            .defXPos      = (unsigned)defpos.x,
            .defYPos      = (unsigned)defpos.y,
            .atkAttribute = atkAttribute,
            .damageDealt  = damageDealt},
        nullptr};
    emitter_.pushEvent(ev);
}

void _AttackManager::sendAttackMissEvent(const AttackInfo& atk)
{
    auto atkpos = atk.atkPosition;
    auto defpos = atk.defPosition;

    EntityEvent ev{
        0,
        EventAttackMiss{
            .attackerID = atk.attackerID,
            .defenderID = atk.defenderID,
            .attackID   = atk.attackID,
            .atkXPos    = (unsigned)atkpos.x,
            .atkYPos    = (unsigned)atkpos.y,
            .defXPos    = (unsigned)defpos.x,
            .defYPos    = (unsigned)defpos.y},
        nullptr};
    emitter_.pushEvent(ev);
}

void _AttackManager::sendDyingEvent(const AttackInfo& atk)
{
    auto defpos = atk.defPosition;

    EntityEvent ev{
        0,
        EventDying{
            .objectID = atk.defenderID,
            .atkXPos    = (unsigned)defpos.x,
            .atkYPos    = (unsigned)defpos.y},
        nullptr};
    emitter_.pushEvent(ev);
}

void _AttackManager::updateAttackInfo(
    const GameObject& attacker, const GameObject& defender, AttackInfo& atk)
{
    auto atkpos     = attacker.getPosition();
    auto defpos     = defender.getPosition();
    atk.atkPosition = glm::vec2(atkpos.x, atkpos.z);
    atk.defPosition = glm::vec2(defpos.x, defpos.z);
}

/**
 * Based on the specified attack precision, check if the
 * next attack will happen
 *
 * TODO: use a global pseudo-RNG. Also make every game have a seed.
 */
bool _AttackManager::isNextAttackPrecise(unsigned precision) const
{
    // clang-format off
    constexpr std::array values = {
        0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
        1, 11, 21, 31, 41, 51, 61, 71, 81, 91, 0,
        2, 12, 22, 32, 42, 52, 62, 72, 82, 92, 1,
    };

    // clang-format on

    static int i = 0;

    bool answer = (values[i]) <= precision;
    i++;

    if (i >= values.size()) i = 0;

    return answer;
}


/**
 * Start a new countdown to the next attack, by incrementing
 * the ticks_until_attack by the attack interval.
 */
void _AttackManager::countUntilNextAttack(AttackInfo& atk)
{
    auto interval = this->getAttackInterval(atk.atkAttributes);
    atk.ticks_until_attack += interval;
}

/**
 * Do the damage, update the projectile positions and other multiple things
 *
 * Runs once per tick
 */
void _AttackManager::update(ObjectManager& om, ObjectLifecycleManager& olm)
{
    std::vector<uint64_t> attacks_to_remove;

    //    printf("%zu attacks\n", attacks_.size());
    for (auto& atk : attacks_) {
        auto atkobj = om.get(atk.attackerID);
        auto defobj = om.get(atk.defenderID);

        if (!defobj || !atkobj) {
            fprintf(stderr, "no ID found??????????????\n");
            attacks_to_remove.push_back(atk.attackID);
            continue;
        }

        double damage = _AttackComponent::calculateDamage(atk.atkAttributes, atk.defAttributes);

        this->updateAttackInfo(*atkobj->get(), *defobj->get(), atk);

        atk.ticks_until_attack--;
        if (atk.ticks_until_attack <= 0.001) {
            int attacks = 0;
            if (isNextAttackPrecise(atk.atkAttributes.precision)) {

                // Fix some precision errors that might occur.
                atk.ticks_until_attack = 0;
                do {
                    auto remaining = this->damageObject(*defobj->get(), damage);
                    this->sendAttackDoneEvent(atk, atk.atkAttributes, damage);

                    if (remaining <= 0) {
                        this->sendDyingEvent(atk);

                        /// The lifecycle manager will delete the entity for us,
                        /// but we will have to sent the dying event.
                        /// TODO: change this, the lifecycle manager will listen to
                        ///       this event.
                        olm.notifyDeath(atk.defenderID);
                        attacks_to_remove.push_back(atk.attackID);
                    }

                    attacks++;
                    this->countUntilNextAttack(atk);
                } while (atk.ticks_until_attack < 1.0);

                atk.successful_attacks += attacks;

            } else {
                this->sendAttackMissEvent(atk);
            }
            atk.total_attacks += attacks;
        }
    }

    auto itend = std::remove_if(attacks_.begin(), attacks_.end(),
                                [&](AttackInfo& a) {
                                    return std::find(attacks_to_remove.begin(),
                                                     attacks_to_remove.end(),
                                                     a.attackID) != attacks_to_remove.end();
                                });
    attacks_.erase(itend, attacks_.end());
}

bool _AttackManager::receiveAttackEvents(const EntityEvent& e)
{

    if (auto* ev = std::get_if<EventAttackStart>(&e.type); ev) {
        AttackInfo a{
            .attackerID         = ev->attackerID,
            .defenderID         = ev->defenderID,
            .attackID           = ev->attackID,
            .rule               = ev->rule,
            .atkAttributes      = ev->atkAttributes,
            .defAttributes      = ev->defAttributes,
            .atkPosition        = glm::vec2(ev->atkXPos, ev->atkYPos),
            .defPosition        = glm::vec2(ev->defXPos, ev->defYPos),
            .successful_attacks = 0,
            .total_attacks      = 0,
            .ticks_until_attack = 0,
        };
        attacks_.push_back(a);
    }

    return true;
}

TEST(AttackManager, BaseMeleeAttack)
{
    _AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    _AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
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
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, TooFarMeleeAttack)
{
    _AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    _AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
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
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, BaseMeleeAttackWithEventPropagation)
{
    ObjectManager om;
    ObjectLifecycleManager olm{om};

    _AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    _AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
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

    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    _AttackManager am;

    auto attack = atk.attack(def);
    ASSERT_TRUE(attack.has_value());

    LogicService::getActionQueue()->processEvents();

    EXPECT_EQ(AttackType::Melee, attack->atype);
    EXPECT_EQ(0.25, attack->value);

    std::queue<EntityEvent> events;
    auto test_recv = [&](const EntityEvent& e) {
        events.push(e);
        return true;
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::AttackMiss});

    EXPECT_EQ(0, events.size());
    am.update(om, olm);
    LogicService::getActionQueue()->processEvents();
    EXPECT_GE(events.size(), 0);
    EXPECT_LE(events.size(), 1);
    am.update(om, olm);
    LogicService::getActionQueue()->processEvents();
    ASSERT_GE(events.size(), 1);
    ASSERT_LE(events.size(), 2);

    auto e   = events.front();
    auto* ev = std::get_if<EventAttackDone>(&e.type);

    {
        auto atkptr = *om.get(atkid);
        auto defptr = *om.get(defid);

        ASSERT_TRUE(ev);
        EXPECT_EQ(atkid, ev->attackerID);
        EXPECT_EQ(defid, ev->defenderID);
        EXPECT_GT(ev->attackID, 0);
        EXPECT_EQ(0.25, ev->damageDealt);
        EXPECT_EQ(200.00, atkptr->getHealth());
        EXPECT_LE(defptr->getHealth(), 199.75);
        EXPECT_GE(defptr->getHealth(), 199.50);
    }

    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, TooFarMeleeAttackWithEventPropagation)
{
    ObjectManager om;
    ObjectLifecycleManager olm{om};

    _AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    _AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
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

    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    _AttackManager am;

    ASSERT_FALSE(atk.isInRange(def));
    auto attack = atk.attack(def);
    ASSERT_FALSE(attack.has_value());
    ASSERT_EQ(AttackError::DefenderTooFar, attack.error());

    std::queue<EntityEvent> events;
    auto test_recv = [&](const EntityEvent& e) {
        events.push(e);
        return true;
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::AttackMiss});

    EXPECT_EQ(0, events.size());
    am.update(om, olm);
    LogicService::getActionQueue()->processEvents();
    EXPECT_EQ(0, events.size());
    am.update(om, olm);
    LogicService::getActionQueue()->processEvents();
    ASSERT_EQ(0, events.size());
    LogicService::getActionQueue()->processEvents();

    {
        auto atkptr = *om.get(atkid);
        auto defptr = *om.get(defid);

        EXPECT_EQ(200.00, atkptr->getHealth());
        EXPECT_EQ(200.00, defptr->getHealth());
    }

    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, IsBaseMeleeAttackNotFullyPreciseIfPrecisionIsLow)
{
    LogicService::getActionQueue()->clearEvents();

    ObjectManager om;
    ObjectLifecycleManager olm{om};

    _AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    _AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
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

    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    _AttackManager am;

    auto attack = atk.attack(def);
    LogicService::getActionQueue()->processEvents();

    auto neededAttacksMin = int((200 / 0.25) / (100.0 / 100)) - 1;
    auto neededAttacksMax = int((200 / 0.25) / (90.0 / 100)) - 1;

    std::queue<EntityEvent> events_done;
    std::queue<EntityEvent> events_miss;
    auto test_recv = [&](const EntityEvent& e) {
        switch (e.type.index()) {
            case ActionQueueEvent::AttackDone: events_done.push(e); return true;
            case ActionQueueEvent::AttackMiss: events_miss.push(e); return true;
            default: return false;
        }
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::AttackMiss});

    for (auto i = 0; i < neededAttacksMin; i++) {
        LogicService::getActionQueue()->processEvents();
        am.update(om, olm);
    }
    LogicService::getActionQueue()->processEvents();

    ASSERT_GT(events_done.size(), 0);
    EXPECT_GE(events_done.size(), neededAttacksMin*0.9);
    EXPECT_LT(events_done.size(), neededAttacksMax);
    EXPECT_GT(events_miss.size(), 0);
    EXPECT_LE(events_miss.size(), neededAttacksMin*0.1);
    EXPECT_GE(events_miss.size(), neededAttacksMin*0.05);

    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, IsBaseMeleeAttackFullyPreciseIfPrecisionIs100)
{
    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();

    ObjectManager om;
    ObjectLifecycleManager olm{om};

    _AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 100,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    _AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
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

    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    _AttackManager am;

    auto attack = atk.attack(def);

    auto neededAttacks = int((200 / 0.25) / (100.0 / 100)) - 1;

    std::queue<EntityEvent> events_done;
    std::queue<EntityEvent> events_miss;
    auto test_recv = [&](const EntityEvent& e) {
        switch (e.type.index()) {
            case ActionQueueEvent::AttackDone: events_done.push(e); return true;
            case ActionQueueEvent::AttackMiss: events_miss.push(e); return true;
            default: return false;
        }
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::AttackMiss});

    for (auto i = 0; i < neededAttacks; i++) {
        LogicService::getActionQueue()->processEvents();
        am.update(om, olm);
    }

    LogicService::getActionQueue()->processEvents();
    ASSERT_EQ(events_done.size(), neededAttacks);
    ASSERT_EQ(events_miss.size(), 0);

    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, IsAttackSpeedRespected)
{
    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();

    ObjectManager om;
    ObjectLifecycleManager olm{om};

    _AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048/16.0,
            .precision     = 100,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    _AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 8,
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

    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    _AttackManager am;

    auto attack = atk.attack(def);

    auto hitAttacks = int((200 / 0.25) / (100.0 / 100)) - 1;
    auto neededIters = int((16*200 / 0.25) / (100.0 / 100)) - 16;

    std::queue<EntityEvent> events_done;
    std::queue<EntityEvent> events_miss;
    auto test_recv = [&](const EntityEvent& e) {
        switch (e.type.index()) {
            case ActionQueueEvent::AttackDone: events_done.push(e); return true;
            case ActionQueueEvent::AttackMiss: events_miss.push(e); return true;
            default: return false;
        }
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::AttackMiss});

    for (auto i = 0; i < neededIters; i++) {
        LogicService::getActionQueue()->processEvents();
        am.update(om, olm);
    }

    LogicService::getActionQueue()->processEvents();
    EXPECT_EQ(events_done.size(), hitAttacks);
    EXPECT_EQ(events_miss.size(), 0);


    {
        auto atkptr = *om.get(atkid);
        auto defptr = *om.get(defid);

        EXPECT_EQ(200.00, atkptr->getHealth());
        EXPECT_EQ(defptr->getHealth(), 0.25);
    }

    
    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}


TEST(AttackManager, IsDeadEventReceivedOnDeath)
{
    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();

    ObjectManager om;
    ObjectLifecycleManager olm{om};

    _AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 100,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    _AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
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


    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    {
        auto atkptr = *om.get(atkid);
        auto defptr = *om.get(defid);

        olm.doRegister(atkptr);
        olm.doRegister(defptr);
    }

    olm.notifyCreation(atkid);
    olm.notifyCreation(defid);
    olm.update();

    _AttackManager am;

    auto attack = atk.attack(def);

    auto neededAttacks = int((200 / 0.25) / (100.0 / 100)) - 1;

    std::queue<EntityEvent> events_done;
    std::queue<EntityEvent> events_dead;
    auto test_recv = [&](const EntityEvent& e) {
        switch (e.type.index()) {
            case ActionQueueEvent::AttackDone: events_done.push(e); return true;
            case ActionQueueEvent::Dying: events_dead.push(e); return true;
            default: return false;
        }
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::Dying});

    for (auto i = 0; i < neededAttacks; i++) {
        LogicService::getActionQueue()->processEvents();
        am.update(om, olm);
        olm.update();
    }
    LogicService::getActionQueue()->processEvents();

    EXPECT_EQ(events_done.size(), neededAttacks);
    EXPECT_EQ(events_dead.size(), 0);

    am.update(om, olm);
    olm.update();
    LogicService::getActionQueue()->processEvents();

    EXPECT_EQ(events_done.size(), neededAttacks+1);
    EXPECT_EQ(events_dead.size(), 1);

    {
        auto atkptr = *om.get(atkid);
        auto defptr = *om.get(defid);

        EXPECT_EQ(200.00, atkptr->getHealth());
        EXPECT_LE(defptr->getHealth(), 0);
    }

    for (auto i = 0; i <= 80*10; i++) {
        LogicService::getActionQueue()->processEvents();
        am.update(om, olm);
        olm.update();

    }

    {
        auto atkptrv = om.get(atkid);
        auto defptrv = om.get(defid);

        EXPECT_TRUE(atkptrv);
        EXPECT_FALSE(defptrv);
    }


    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}

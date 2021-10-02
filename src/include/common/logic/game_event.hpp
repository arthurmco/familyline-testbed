#pragma once

#include <memory>
#include <queue>
#include <string>
#include <variant>

namespace familyline::logic
{
// TODO: rename GameObject to GameEntity?

/*
 * These events are events that happen from the point of view of the
 * objects
 *
 * They exist so other systems can know what happened to a certain
 * entity without having to query its attributes all the time.
 *
 * Following are the event struct types.
 * Because C++ lack sum types, we need to create separate structs to each
 * event type and glue them using an std::variant. This is, honestly,
 * one of the things that I like Rust added, and I would prefer to do
 * this in Rust.
 */
typedef unsigned long long entity_id_t;

/**
 * An object has been created.
 *
 * It does not means unit born, or construction built.
 * It only means the object has been added to the object manager
 */
struct EventCreated {
    entity_id_t objectID;
};

/**
 * An entity has been started to build
 *
 * This means a building has been started to be built.
 * This event also happens for units, but right before the "Built" event
 */
struct EventBuilding {
    entity_id_t objectID;
};

/**
 * An entity has been built
 *
 * This means a building finished build, or an unit has been born
 */
struct EventBuilt {
    entity_id_t objectID;
};

/**
 * The entity has been finished doing some action and it is waiting
 */
struct EventReady {
    entity_id_t objectID;
};


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

    
/**
 * An entity initiated an attacker
 *
 * We have the attacker, the defender, and an unique ID that will identify this
 * attack attempt, so you can reliably cross-reference it with the corresponding
 * EventAttackMiss or EventAttackDone
 *
 * You also have the position of the attacker and the defender when the event
 * was sent to the queue.
 */
struct EventAttackStart {
    entity_id_t attackerID;
    entity_id_t defenderID;

    uint64_t attackID;

    AttackRule rule;
    AttackAttributes atkAttributes;
    AttackAttributes defAttributes;
    
    unsigned int atkXPos, atkYPos;
    unsigned int defXPos, defYPos;
};

/**
 * An entity missed an attack
 *
 * You also have the position of the attacker and the defender when the event
 * was sent to the queue (might not be the same as the corresponding
 * EventAttackStart)
 */
struct EventAttackMiss {
    entity_id_t attackerID;
    entity_id_t defenderID;

    uint64_t attackID;

    unsigned int atkXPos, atkYPos;
    unsigned int defXPos, defYPos;
};

/**
 * An entity has done an attack
 *
 * One smart note: since like, in real life, when you gonna attack someone
 * (like, for example, an asshole that called you a slur), and hit, without
 * wanting to, some of your friends, the `defender` might not be the same
 * one as the defender in the corresponding EventAttackStart. You might
 * as well receive multiple attacks with the same attackID
 *
 * You also have the used attack definition (the attack component attributes
 * used to calculate the damage dealt), the damage dealt itself and the
 * position of the attacker and the defender when the event was sent to the
 * queue (might not be the same as the corresponding EventAttackStart)
 */
struct EventAttackDone {
    entity_id_t attackerID;
    entity_id_t defenderID;

    uint64_t attackID;

    unsigned int atkXPos, atkYPos;
    unsigned int defXPos, defYPos;

    AttackAttributes atkAttribute;
    double damageDealt;
};

/**
 * The entity who sent the event is attacking another entity
 *
 * We also send the position of where the attacker and the defender was
 */
struct EventAttacking {
    entity_id_t attackerID;
    entity_id_t defenderID;

    unsigned int atkXPos, atkYPos;
    unsigned int defXPos, defYPos;

    double damageDealt;
};

/**
 * This entity is working, collecting resources
 */
struct EventWorking {
    entity_id_t objectID;

    unsigned int atkXPos, atkYPos;
};

/**
 * This entity is starting to get garrisoned or ungarrisoned
 *
 * The `parentID` is the entity that you will be entering/exiting, and
 * the `entering` value defines if you are entering to/exiting from the
 * entity.
 */
struct EventGarrisoned {
    entity_id_t objectID;

    entity_id_t parentID;
    bool entering;
};

/**
 * This entity just had its health points reduced to 0
 *
 * It started dying.
 * This event will be used to run the death animation
 */
struct EventDying {
    entity_id_t objectID;

    unsigned int atkXPos, atkYPos;
};

/**
 * This entity just ended the death animation. Only its remains lie at the
 * floor.
 *
 * Soon this entity will be cleaned from memory
 */
struct EventDead {
    entity_id_t objectID;
};

/**
 * The entity has been deallocated from memory
 *
 * You will not be able to use this ID. This is only for you to know
 * who has been deallocated, so you can remove this ID from *your*
 * bookkeeping lists
 */
struct EventDestroyed {
    entity_id_t objectID;
};

class EventEmitter;
class ActionQueue;

/**
 * The event type array
 *
 * PLEASE keep these types synced with the enum ActionQueeEvent in <common/logic/action_queue.hpp>,
 * so the listeners can know what actions to listen
 *
 * If you keep them desynchronized (with the .index() of the variant not corresponding to
 * the same-named ActionQueueEvent member), the listened event types will not match the real
 * events.
 */
typedef std::variant<
    EventCreated, EventBuilding, EventBuilt, EventReady, EventAttackStart, EventAttackMiss,
    EventAttackDone, EventAttacking, EventWorking, EventGarrisoned, EventDying, EventDead,
    EventDestroyed>
    EntityEventType;

struct EntityEvent {
    unsigned long long timestamp;
    EntityEventType type;
    EventEmitter* emitter;
};

/**
 * The event emitter class
 *
 * Will be used like a proxy so you can send events to the action queue
 */
class EventEmitter {
public:
    EventEmitter(std::string name) :
        name_(name)
        {}    

    /**
     * Pushes the event to the central action queue
     */
    void pushEvent(EntityEvent& e);

    virtual ~EventEmitter() {}
    ActionQueue* queue;

    std::string_view getName() const { return name_; }
protected:
    std::string name_;

    
};

    
}  // namespace familyline::logic

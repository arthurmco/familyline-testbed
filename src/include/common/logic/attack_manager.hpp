#pragma once

#include <common/logic/game_event.hpp>
#include <common/logic/lifecycle_manager.hpp>
#include <common/logic/object_components.hpp>
#include <unordered_map>

namespace familyline::logic
{
/**
 * The attack manager
 *
 * Causes the real damage, calculate  the damage of the projectiles, etc.
 */
class AttackManager
{
public:
    AttackManager();
    ~AttackManager();

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

}  // namespace familyline::logic

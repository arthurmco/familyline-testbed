#pragma once

#include <unordered_map>
#include "object_components.hpp"
#include "game_event.hpp"

namespace familyline::logic {

/**
 * Manages the attacking and defensing of all objects
 *
 * Will also issue attacking and defending events when those 
 * actions occur
 */

    typedef uint64_t attack_handle_t;

    struct AttackData {
        AttackComponent* atk;
        AttackComponent* def;

        // Delay until the next attack, in game ticks
        int atk_tick_delay = 0;
    };

    class AttackManagerEventReceiver : public EventReceiver
    {
    private:
        std::string _name = "atk-manager-event-receiver";
    public:
        AttackManagerEventReceiver();

        virtual const std::string getName();

        virtual ~AttackManagerEventReceiver() {}
    };


    class AttackManagerEventEmitter : public EventEmitter
    {
    private:
        std::string _name = "atk-manager-event-emitter";
    public:
        AttackManagerEventEmitter();

        virtual const std::string getName();

        void generateAttackEvent(AttackComponent* atk, AttackComponent* def, double dmg);

        virtual ~AttackManagerEventEmitter() {}
    };


    class AttackManager {
    private:
        std::unordered_map<int /* object id */, AttackComponent*> components;
        std::unordered_map<attack_handle_t, AttackData> attacks;

        /**
         * Attack the 'defender`
         *
         * Also modifies the defender HP and armor by decreasing both
         */
        std::optional<double> attack(AttackComponent& defender);

        /**
         * Check if any object has been deleted and should be removed
         */
        void checkRemovedObjects();
        AttackManagerEventReceiver* ame_receiver;
        AttackManagerEventEmitter* ame_emitter;
    public:

        AttackManager();
        ~AttackManager();
        
        void doRegister(int oid, AttackComponent& atk);

        /**
         * Start an attack action from attackerOID against defenderOID
         *
         * It will not start now, but will be deferred later, so it can
         * process all attacks for this tick at once
         *
         * Returns an 'attack handle' that you can store to stop the attack
         */
        attack_handle_t startAttack(int attackerOID, int defenderOID);


        /**
         * End an attack.
         *
         * Attacks also automatically ends when the attacked unit is out of range
         */
        void endAttack(attack_handle_t ahandle);


        /**
         * Process all scheduled attacks
         */
        void processAttacks();

    };

/**
 * Create an attack handle from the attacked and the defended unit OIDs
 */
    attack_handle_t make_attack_handle(int attackerOID, int defenderOID);

/**
 * Split an attack handle to the attacker and defender
 */
    std::tuple<int, int> break_attack_handle(attack_handle_t handle);
}

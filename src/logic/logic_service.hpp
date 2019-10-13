#pragma once

#include <memory>
#include "action_queue.hpp"
#include "object_manager.hpp"
#include "attack_manager.hpp"
#include "object_listener.hpp"

/**
 * Logic service class
 */
namespace familyline::logic {

    class LogicService {
    private:
        static std::unique_ptr<ActionQueue> _action_queue;
        static std::unique_ptr<AttackManager> _atk_manager;
        static std::unique_ptr<ObjectListener> _object_listener;

    public:
        static std::unique_ptr<ActionQueue>& getActionQueue();
        static std::unique_ptr<AttackManager>& getAttackManager();
        static std::unique_ptr<ObjectListener>& getObjectListener();
    };

}

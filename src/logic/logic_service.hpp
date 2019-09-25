#pragma once

#include <memory>
#include "action_queue.hpp"

/**
 * Logic service class
 */
namespace familyline::logic {

    class LogicService {
    private:
        static std::unique_ptr<ActionQueue> _action_queue;


    public:
        static std::unique_ptr<ActionQueue>& getActionQueue();

    };

}

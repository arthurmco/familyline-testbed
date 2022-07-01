#include "test_inputprocessor.hpp"

using namespace familyline::input;

/**
 * Get the next action
 *
 * Return true if we have a next action, false if the action queue is
 * empty
 */
bool TestInputProcessor::pollAction(HumanInputAction& a)
{
    if (actions_.empty())
        return false;

    a = actions_.front();
    actions_.pop();
    return true;
}

void TestInputProcessor::pushAction(familyline::input::HumanInputAction action)
{
    actions_.push(action);
}

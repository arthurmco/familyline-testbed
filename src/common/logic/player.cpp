#include <common/logic/player.hpp>

using namespace familyline::logic;

void Player::pushToBuildQueue(std::string type)
{
    build_queue_.push(type);
}

void Player::clearBuildQueue()
{
    while (!build_queue_.empty())
        build_queue_.pop();
}

/**
 * Builds the next available object in the build queue
 */
void Player::buildNext()
{
    // TODO: create an action to build the one that is on the top of the stack
}


/**
 * Process the player input actions
 *
 * They might do things like moving a unit, or attacking someone,
 * or running some action
 *
 * They will receive data from the input manager.
 */
void Player::processActions()
{
    
}


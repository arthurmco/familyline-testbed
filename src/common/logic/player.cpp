#include <common/logic/player.hpp>
#include <common/logic/player_manager.hpp>

using namespace familyline::logic;



void Player::pushAction(PlayerInputType type) {
    this->pm_.pushAction(this->code_, type);
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


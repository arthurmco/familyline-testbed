#include <common/logic/player.hpp>
#include <common/logic/player_manager.hpp>
#include <common/logger.hpp>

using namespace familyline::logic;

void Player::pushAction(PlayerInputType type) {
    this->pm_.pushAction(this->code_, type);
}

void Player::pushToSelection(unsigned object_id, std::weak_ptr<GameObject> o)
{
    auto& log = LoggerService::getLogger();
    if (o.expired()) {
        log->write("player", LogType::Error, "tried to push a removed object (id %d)", object_id);
    }    

    puts("oh");
    std::shared_ptr<GameObject> so = o.lock();
    assert(so->getID() == object_id);
    selected_.push_back(so);
}

void Player::popFromSelection(unsigned object_id)
{
    auto new_end = std::remove_if(
        selected_.begin(), selected_.end(),
        [&](auto& o) {           
            if (o.expired()) {
                return true;
            }
            
            return o.lock()->getID() == object_id;
        });

    selected_.erase(new_end);
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


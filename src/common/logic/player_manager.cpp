#include <common/logic/player_manager.hpp>
#include <chrono>

using namespace familyline::logic;

/**
 * Add a player here
 * 
 * Return its generated ID
 */
int PlayerManager::add(std::unique_ptr<Player> p)
{
    auto& pi = players_.emplace_back((uintptr_t)p.get() / ((uintptr_t)players_.size()*16384),
                                     std::move(p));
    return pi.id;
}

/**
 * Push an action
 */
void PlayerManager::pushAction(int id, PlayerInputType type)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
	uint64_t micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

    PlayerInputAction a;
    a.playercode = id;
    a.tick = _tick;
    a.timestamp = micros;
    a.type = type;

    actions_.push(a);
}

/**
 * Adds a listener to the player input action event listeners
 *
 * Returns the ID
 */
int PlayerManager::addListener(PlayerListenerHandler h)
{
    PlayerHandlerInfo phi;
    phi.id = player_input_listeners_.size() + 1;
    phi.handler = h;

    player_input_listeners_.push_back(phi);
}

/**
 * Run the input handlers and pop the event from the input action
 * queue
 */
void PlayerManager::run(unsigned int tick)
{
    _tick = tick;
    if (!actions_.empty()) {
        PlayerInputAction& a = actions_.front();

        for (auto h : player_input_listeners_) {
            h.handler(a);
        }

        actions_.pop();
    }
}


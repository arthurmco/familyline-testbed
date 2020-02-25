#include <client/input/input_manager.hpp>

using namespace familyline::input;

InputManager::InputManager(InputProcessor& ip)
    : _ip(ip)
{
    ip.startInputReceiver();
}


/**
 * Process input events, human inputs and player inputs
 *
 * The human inputs come from the input processor
 * The player input come from the player who call `queuePlayerInput`
 */
void InputManager::processEvents()
{
    _tick++;

    HumanInputAction hia;

    while (_ip.pollAction(hia)) {
        for (auto& [id, fn] : _human_input_fns) {
            auto ret = fn(hia);

            if (ret) {
                break;
            }
        }
        
    }
    
}


/**
 * Enqueue a player input
 */
void InputManager::queuePlayerInput(PlayerInputType type, int playerCode)
{
    
}

int generateHandlerNumber()
{
    static int id = 0;
    id++;

    return id;
}

/**
 * Adds a listener handler, returns an ID
 */
listener_handler_t InputManager::addListenerHandler(HumanListenerHandler h)
{
    auto id = generateHandlerNumber();
    _human_input_fns[id] = h;
    return id;
}

listener_handler_t InputManager::addListenerHandler(PlayerListenerHandler h)
{
    auto id = generateHandlerNumber();
    _player_input_fns[id] = h;
    return id;
}


void InputManager::removeListenerHandler(listener_handler_t id)
{
    
}

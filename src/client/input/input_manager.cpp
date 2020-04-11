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
        for (const auto& v: _human_input_fns) {
            auto ret = v.handler(hia);

            if (ret) {
                break;
            }
        }
        
    }
    
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
listener_handler_t InputManager::addListenerHandler(HumanListenerHandler h, int zorder)
{
    auto id = generateHandlerNumber();
	auto ho = HandlerOrder<HumanListenerHandler>{ h, id, zorder };

	int insertidx = 0;
	for (int i = 0; i < _human_input_fns.size(); i++) {
		if (_human_input_fns[i].zindex < zorder) {
			break;
		}
		

		insertidx++;
	}

	_human_input_fns.insert(_human_input_fns.begin() + insertidx, ho);

    return id;
}

void InputManager::removeListenerHandler(listener_handler_t id)
{
    
}

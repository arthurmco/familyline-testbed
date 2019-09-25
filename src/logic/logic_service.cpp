#include "logic_service.hpp"

using namespace familyline::logic;

std::unique_ptr<ActionQueue> LogicService::_action_queue;

std::unique_ptr<ActionQueue>& LogicService::getActionQueue()
{
	if (!_action_queue) {
		_action_queue = std::make_unique<ActionQueue>();
	}

	return _action_queue;
}

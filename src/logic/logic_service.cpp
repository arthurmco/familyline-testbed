#include "logic_service.hpp"

using namespace familyline::logic;

std::unique_ptr<ActionQueue> LogicService::_action_queue;
std::unique_ptr<AttackManager> LogicService::_atk_manager;

std::unique_ptr<ActionQueue>& LogicService::getActionQueue()
{
	if (!_action_queue) {
		_action_queue = std::make_unique<ActionQueue>();
	}

	return _action_queue;
}

std::unique_ptr<AttackManager>& LogicService::getAttackManager()
{
	if (!_atk_manager) {
		_atk_manager = std::make_unique<AttackManager>();
	}

	return _atk_manager;
}

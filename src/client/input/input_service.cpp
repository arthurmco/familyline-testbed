#include <client/input/input_service.hpp>

using namespace familyline::input;

std::unique_ptr<InputManager> InputService::_input_manager;

std::unique_ptr<InputManager>& InputService::getInputManager()
{
    return _input_manager;
}

std::unique_ptr<InputManager>& InputService::setInputManager(
    std::unique_ptr<InputManager> v)
{
    _input_manager.swap(v);
}

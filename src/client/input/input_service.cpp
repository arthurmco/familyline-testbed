#include <client/input/input_service.hpp>

using namespace familyline::input;

std::unique_ptr<InputManager> InputService::input_manager_;

std::unique_ptr<InputManager>& InputService::getInputManager() { return input_manager_; }
void InputService::setInputManager(std::unique_ptr<InputManager> v) { input_manager_.swap(v); }

std::unique_ptr<CommandTable> InputService::command_table_;

std::unique_ptr<CommandTable>& InputService::getCommandTable() { return command_table_; }
void InputService::setCommandTable(std::unique_ptr<CommandTable> v) { command_table_.swap(v); }

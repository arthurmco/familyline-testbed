#pragma once

#include <memory>

#include <client/input/input_manager.hpp>
#include <client/input/command_table.hpp>

/**
 * Input service class
 */

namespace familyline::input
{
class InputService
{
private:
    static std::unique_ptr<InputManager> input_manager_;
    static std::unique_ptr<CommandTable> command_table_;

public:
    static std::unique_ptr<InputManager>& getInputManager();
    static void setInputManager(std::unique_ptr<InputManager> v);

    static std::unique_ptr<CommandTable>& getCommandTable();
    static void setCommandTable(std::unique_ptr<CommandTable> c);
};

}  // namespace familyline::input

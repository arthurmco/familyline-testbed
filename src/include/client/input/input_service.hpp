#pragma once

#include <memory>
#include "input_manager.hpp"

/**
 * Input service class
 */

namespace familyline::input {

    class InputService {
    private:
        static std::unique_ptr<InputManager> _input_manager;


    public:
        static std::unique_ptr<InputManager>& getInputManager();
        static void setInputManager(std::unique_ptr<InputManager> v);
        
    };    
    
}



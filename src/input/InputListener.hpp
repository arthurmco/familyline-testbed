/*
    Input listening class

    Copyright (C) 2016 Arthur M
*/

#include <queue>
#include "InputEvent.hpp"

#ifndef INPUTLISTENER_HPP
#define INPUTLISTENER_HPP

namespace Tribalia {
namespace Input {

class InputListener {
private:
    std::queue<InputEvent> _events;

public:
    /* Called only by the InputManager class when you receive a wished event */
    void OnListen(InputEvent e);

    /* Return true if there's an event, and copy its data to ev */
    bool PopEvent(InputEvent& ev);
};

}
}

#endif /* end of include guard: INPUTLISTENER_HPP */

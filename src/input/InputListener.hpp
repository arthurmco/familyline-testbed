/*
    Input listening class

    Copyright (C) 2016 Arthur M
*/

#include <queue>
#include "InputEvent.hpp"

#ifndef INPUTLISTENER_HPP
#define INPUTLISTENER_HPP

namespace familyline::input {

class InputListener {
private:
    std::queue<InputEvent> _events;
    bool inputAccepted = false;
    char* name;

public:
    InputListener(const char* name = "default");

    /**
     * Called only by the InputManager class when you receive a wished event
     */
    void OnListen(InputEvent e);

    /**
     * Return true if there's an event, and copy its data to ev
     */
    bool PopEvent(InputEvent& ev);

    /**
     * Call if you 'accept' the input, i.e, you already took care of it and
     * no one should take care
     */
    void SetAccept();

    /**
     * Retrieves (and resets) input acception status
     */
    bool GetAcception();

    char* GetName() const ;
    void SetName(char*);
};

}

#endif /* end of include guard: INPUTLISTENER_HPP */

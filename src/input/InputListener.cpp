#include "InputListener.hpp"

using namespace Tribalia::Input;

/* Called only by the InputManager class when you receive a wished event */
void InputListener::OnListen(InputEvent e)
{
    _events.push(e);
}

/* Return true if there's an event, and copy its data to ev */
bool InputListener::PopEvent(InputEvent& ev)
{
    if (_events.empty()) {
        return false;
    }

    ev = _events.front();
    _events.pop();
    return true;
}

/* Call if you 'accept' the input, i.e, you already took care of it and
    no one should take care */
void InputListener::SetAccept()
{
    inputAccepted = true;
}

/* Retrieves (and resets) input acception status */
bool InputListener::GetAcception()
{
    bool r = inputAccepted;
    inputAccepted = false;
    return r;
}

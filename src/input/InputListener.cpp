#include "InputListener.hpp"
#include "Log.hpp"

using namespace Familyline::Input;

InputListener::InputListener(const char* name)
{
    this->name = const_cast<char*>(name);
    Log::GetLog()->Write("input-listener", "Created input listener '%s'",
			 name);
}
    

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

char* InputListener::GetName() const
{
    return this->name;
}

void InputListener::SetName(char* n)
{
    this->name = n;
}

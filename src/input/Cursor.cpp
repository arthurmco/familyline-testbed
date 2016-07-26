#include "Cursor.hpp"

using namespace Tribalia::Input;

Cursor* Cursor::cursor = nullptr;

void Cursor::GetPositions(int& x, int& y)
{
    InputEvent ev;
    if (_listener->PopEvent(ev)) {
        _x = ev.mousex;
        _y = ev.mousey;
    }

    x = _x;
    y = _y;
}

Cursor::Cursor()
{
    _listener = new InputListener{};
    InputManager::GetInstance()->AddListener(EVENT_MOUSEMOVE, _listener);
}

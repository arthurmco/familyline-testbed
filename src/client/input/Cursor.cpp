#include <client/input/Cursor.hpp>

using namespace familyline::input;

Cursor* Cursor::cursor = nullptr;

void Cursor::GetPositions(int& x, int& y)
{
    x = _x;
    y = _y;
}

Cursor::Cursor()
{
  _listener =
      [&](HumanInputAction hia) {
        if (std::holds_alternative<MouseAction>(hia.type)) {
          auto event = std::get<MouseAction>(hia.type);

          _x = event.screenX;
          _y = event.screenY;

          // We received the event, but we also want the event to pass through normally
          // so we return false anyway
        }

        return false;
      };

  input::InputService::getInputManager()->addListenerHandler(_listener, 20);

  //    InputManager::GetInstance()->AddListener(EVENT_MOUSEMOVE, _listener,
  //    0.01);
}

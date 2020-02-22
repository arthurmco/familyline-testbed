#include <client/input/input_processor.hpp>
#include <chrono>
#include <SDL2/SDL.h>

using namespace familyline::input;

void InputProcessor::startInputReceiver()
{
    _runThread = std::thread(
        [&]() {
            _isRunning = true;

            int lastX, lastY;

            SDL_Event e;
            while (_isRunning) {
                if (!SDL_PollEvent(&e)) {
                    SDL_Delay(20);
                    continue;
                }

                auto duration =  std::chrono::system_clock::now().time_since_epoch();
                uint64_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

                switch (e.type) {
                case SDL_MOUSEMOTION:
                    lastX = e.motion.x;
                    lastY = e.motion.y;
                    _actions.push({millis, MouseAction{e.motion.x, e.motion.y}});
                    break;

                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP: {
                    bool isPressed = (e.button.state == SDL_PRESSED);
                    _actions.push({millis, ClickAction{e.motion.x, e.motion.y, e.button.button,
                                                       e.button.clicks, isPressed}});
                }
                    break;
                case SDL_MOUSEWHEEL: {
                    bool dirNormal = e.wheel.direction == SDL_MOUSEWHEEL_NORMAL;
                    _actions.push({millis, WheelAction{
                                lastX, lastY,
                                (dirNormal) ? e.wheel.x : -e.wheel.x,
                                (dirNormal) ? e.wheel.y : -e.wheel.y,
                            }});
                }
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    bool isPressed = (e.key.state == SDL_PRESSED);
                    bool isRepeated = (e.key.repeat > 0);
                    uint16_t modifiers = e.key.keysym.mod;
                    auto keycode = e.key.keysym.sym;                    
                    _actions.push({millis, KeyAction{
                            keycode, SDL_GetKeyName(keycode),
                            isPressed, isRepeated, modifiers
                            }});

                }
                    break;
                    
                case SDL_QUIT:
                    fprintf(stderr, "event: GameExit\n");
                    _actions.push({millis, GameExit{0}});
                    _isRunning = false;
                    break;
                default:
                    fprintf(stderr, "event id %08x\n", e.type);
                }

            }

        });
}

void InputProcessor::stopInputReceiver()
{
    _isRunning = false;
    _runThread.join();
}


bool InputProcessor::pollAction(HumanInputAction& a)
{
    if (_actions.empty())
        return false;

    a = _actions.front();
    _actions.pop();
    return true;
}


/***
    Input management class

    Copyright 2016 Arthur M.

***/

#include <queue>
#include <SDL2/SDL.h>

#include "../Log.hpp"

#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

namespace Tribalia {
namespace Input {

    enum EventType {
        EVENT_KEYEVENT,     /* Key event */
        EVENT_MOUSEMOVE,
        EVENT_MOUSEEVENT,   /* Mouse event */
        EVENT_FINISH,       /* Finish request (i.e window closing) */
    };

    enum MouseButton {
        MOUSE_LEFT,
        MOUSE_MIDDLE,
        MOUSE_RIGHT,
    };

    enum KeyStatus {
        KEY_KEYPRESS,
        KEY_KEYRELEASE,
        KEY_KEYREPEAT,
    };

    struct InputEvent {
        //Event type (key or mouse event)
        int eventType;

        //Mouse X, Y and Z (last one reserved for holographic displays)
        int mousex, mousey, mousez;

        union {
            struct {
                int button;
                int status;
                int reserved; //for holo display things
            } mouseev;
            struct {
                int scancode;
                int status;
                int char_utf8;
            } keyev;
        } event;
    };

    class InputManager
    {
    private:
        std::queue<InputEvent> _evt_queue;

    public:
        /* Get the top event (not taking it off the queue).
            Return false if no elements on queue */
        bool GetEvent(InputEvent* ev);

        /* Pop off the top element of the queue
            Return false if there's no element to pop off */
        bool PopEvent(InputEvent*);

        /* Receive events and send them to queues */
        void Run();
    };

} /* Input */
} /* Tribalia */



#endif /* end of include guard: INPUTMANAGER_HPP */

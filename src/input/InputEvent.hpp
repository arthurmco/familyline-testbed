/*
    Input event data

    Copyright (C) 2016 Arthur M
*/

#ifndef INPUTEVENT_HPP
#define INPUTEVENT_HPP

namespace Tribalia {
namespace Input {

enum EventType {
    EVENT_KEYEVENT = 1,     /* Key event */
    EVENT_MOUSEMOVE = 2,
    EVENT_MOUSEEVENT = 4,   /* Mouse event */
    EVENT_FINISH = 8,       /* Finish request (i.e window closing) */
    EVENT_ALL_EVENTS = -1
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

}
}

#endif /* end of include guard: INPUTEVENT_HPP */

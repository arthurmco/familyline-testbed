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
    EVENT_DEFAULT_LISTENER = 65535,
    EVENT_ALL_EVENTS = -1
};

enum MouseButton {
    MOUSE_LEFT = 1,
    MOUSE_MIDDLE,
    MOUSE_RIGHT,
};

enum KeyStatus {
    KEY_KEYPRESS = 1,
    KEY_KEYRELEASE,
    KEY_KEYREPEAT,
};

struct InputEvent {    
    // Event ID
    unsigned int eventid;
    
    //Event type (key or mouse event)
    int eventType;

    //Is event paired with another event?
    bool isPaired;
    
    //Mouse X, Y and Z (last one reserved for holographic displays)
    int mousex, mousey, mousez;
    union {
        struct {
            int button;
            int status;
	    short scrollx, scrolly;
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

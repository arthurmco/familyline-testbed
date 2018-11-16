/* GUI signal and signal types
 *
 *  A signal is simply a message one GUI control can send to another
 *
 *  Copyright (C) 2018 Arthur Mendes
 *
 */

#ifndef GUISIGNAL_H
#define GUISIGNAL_H

namespace familyline::graphics::gui {

    class GUIControl;
    
    enum SignalType {
	// External events (mouse/keyboard)
	MouseClick = 1,
	MouseHover,
    
	KeyDown,

	// Internal events
	Redraw = 0x81,
	Timer,
	TimerEnd,

	// When a child gets added to a parent. Useful to get parent sizes
	ContainerAdd = 0xE1,
    };

    /**
     * The signals
     *
     * Each control can receive a signal, for something that happens in the real world,
     * like a button click
     * The control will react to that and emit an event
     */
    struct GUISignal {
	const GUIControl *from, *to;

	SignalType signal;

	float xPos, yPos;

	/// Event extra parameters
	int param[2];

	union {
	    // Group 1: Key/mouse/joystick
	    struct {
		char* keystr; ///< Key string representation
		unsigned keycode;
	    } key;
	    
	    struct {
		unsigned button; ///< What button? (1 = Left, 2 = Right, 3 = Middle)
		bool isPressed;
		
	    } mouse;
	    

	    // Group 2: Timer event
	    struct {
		/// Send a timer event. After 'msec' miliseconds, receives a TimerEnd event.
		/// Useful for events that need a little time, like a selection effect
		unsigned mseconds;
		char* tag;
	    } timer;
	
	    // Group 3: Container add, redraw
	    struct {
		/// Absolute size, in pixels, of the screen
		unsigned absw, absh;
	    };
	};
    };

    
}


#endif /* GUISIGNAL_H */

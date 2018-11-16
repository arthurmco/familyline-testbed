
/***
    Input management class

    Copyright 2016 Arthur Mendes.

***/

#include <queue>
#include <vector>
#include <map>
#include <SDL2/SDL.h>

#include "InputEvent.hpp"
#include "InputListener.hpp"

#include "Log.hpp"

#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

namespace familyline::input {

    /**
     * Maximum events for each iteration.
     */
    #define MAX_INPUT_QUEUE 128

    struct InputListenerData {
        int type_mask;  ///< Mask of events you want to receive info
        InputListener* listener;
	float order;
    };

    struct InputListenerMap {
	InputEvent ev;
	InputListener* l;

	inline InputListenerMap(InputEvent e, InputListener* lis)
	    : ev(e), l(lis) {}

	inline InputListenerMap()
	    : InputListenerMap(InputEvent{}, nullptr) {}
    };

    class InputManager
    {
    private:
        std::queue<InputEvent> _evt_queue;
        std::vector<InputListenerData> _listeners;

	/**
	 * Maps event IDs to input listeners, so we can remember what
	 * listener the event was sent, and 'close' it.
	 */
	std::map<unsigned int, InputListenerMap> listener_map;

        static InputManager* im;

	/**
	 * The default listener will catch the remaining events that other listeners didn't
	 */
	InputListener* default_listener = nullptr;
	InputListener* current_listener = nullptr;
	unsigned int _last_eid = 0;

	unsigned int FindEIDForKeyEvent(InputEvent& ev);
	unsigned int FindEIDForMouseEvent(InputEvent& ev);

	int lastx, lasty, lastz;
	std::vector<int> lastscancodes;
	unsigned int last_motion_timestamp = 0;

	/**
	 * Convert the SDL event in the event queue to Familyline events
	 */
	void ConvertEvents();

    public:
	void Initialize();

        /**
	 * Get the top event (not taking it off the queue)
	 * \return false if no elements on queue
	 */
        bool GetEvent(InputEvent* ev);

        /**
	 * Pop off the top element of the queue
	 * \return false if there's no element to pop off
	 */
        bool PopEvent(InputEvent*);

        void AddListener(int types, InputListener* listener, float order=1.0);
        void RemoveListener(InputListener* listener);

	InputListener* GetDefaultListener();

	/**
	 * Retrieve the listener that is currently receiving events
	 */
	InputListener* GetCurrentListener();

        /**
	 * Receive events and send them to queues
	 */
        void Run();

        static InputManager* GetInstance() {
            if (!im) im = new InputManager{};

            return im;
        }
    };

} /* familyline::input */

#endif /* end of include guard: INPUTMANAGER_HPP */

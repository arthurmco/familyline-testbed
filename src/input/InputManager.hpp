
/***
    Input management class

    Copyright 2016 Arthur M.

***/

#include <queue>
#include <vector>
#include <SDL2/SDL.h>

#include "InputEvent.hpp"
#include "InputListener.hpp"

#include "../Log.hpp"

#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

namespace Tribalia {
namespace Input {

    /*  Maximum events for each iteration. */
    #define MAX_INPUT_QUEUE 60

    struct InputListenerData {
        int type_mask;  /* Mask of events you want to receive info */
        InputListener* listener;
    };

    class InputManager
    {
    private:
        std::queue<InputEvent> _evt_queue;
        std::vector<InputListenerData> _listeners;

        static InputManager* im;

		/* The default listener will catch the remaining events that other listeners didn't */
		InputListener* default_listener;
    public:
		void Initialize();

        /* Get the top event (not taking it off the queue).
            Return false if no elements on queue */
        bool GetEvent(InputEvent* ev);

        /* Pop off the top element of the queue
            Return false if there's no element to pop off */
        bool PopEvent(InputEvent*);

        void AddListener(int types, InputListener* listener);
        void RemoveListener(InputListener* listener);

		InputListener* GetDefaultListener();

        /* Receive events and send them to queues */
        void Run();

        static InputManager* GetInstance() {
            if (!im) im = new InputManager{};

            return im;
        }
    };

} /* Input */
} /* Tribalia */



#endif /* end of include guard: INPUTMANAGER_HPP */

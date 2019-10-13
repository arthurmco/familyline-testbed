#pragma once

#include "game_event.hpp"

#include <queue>
#include <vector>

namespace familyline::logic {

    struct ReceiverData {
        EventReceiver* receiver;
        std::vector<EventType> events;
    };

    /**
     * Acts like a central hub for events
     */
    class ActionQueue {
    private:
        std::queue<Event> events;
        std::vector<ReceiverData> receivers;

    public:
        void addEmitter(EventEmitter* e);
        void addReceiver(EventReceiver* r, std::initializer_list<EventType> events);
	
        void pushEvent(const Event& e);
        void processEvents();
    };

}

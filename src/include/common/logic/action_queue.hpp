#pragma once

#include <common/logic/game_event.hpp>
#include <queue>
#include <vector>

namespace familyline::logic
{

/**
 * The type of event
 *
 * Please keep the queue event enumerator elements in the same order as the
 * EntityEventType variant in the game_event.hpp header, because I use this
 * value as sort of a numeric value 
 */
enum ActionQueueEvent {
    Created = 0,
    Building,
    Built,
    Ready,
    Attacking,
    Working,
    Garrisoned,
    Dying,
    Dead,
    Destroyed
};

struct ReceiverData {
    EventReceiver* receiver;
    std::vector<ActionQueueEvent> events;
};

/**
 * Acts like a central hub for events
 */
class ActionQueue
{
private:
    std::queue<EntityEvent> events;
    std::vector<ReceiverData> receivers;

public:
    void addEmitter(EventEmitter* e);
    void addReceiver(EventReceiver* r, std::initializer_list<ActionQueueEvent> events);

    void removeReceiver(EventReceiver*);
    void removeEmitter(EventEmitter*);

    void pushEvent(const EntityEvent& e);
    void processEvents();

    void clearEvents()
    {
        while (!events.empty()) {
            events.pop();
        }
    }
};

}  // namespace familyline::logic

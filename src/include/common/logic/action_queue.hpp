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
 *
 * TODO: actually enforce this, like this:
 * https://en.cppreference.com/w/cpp/utility/variant/variant_alternative
 */
enum ActionQueueEvent {
    Created = 0,
    Building,
    Built,
    Ready,
    AttackStart,
    AttackMiss,
    AttackDone,
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
 *
 * TODO: Use simple functors as receivers/emitters, not full classes
 *       This is not Java 7
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

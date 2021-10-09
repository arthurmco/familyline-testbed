#pragma once

#include <common/logic/game_event.hpp>
#include <functional>
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

/**
 * The event receiving callback.
 *
 * If you register a callback in the action queue, everytime a new event happens
 * that has one of the types you asked for, this function will be called
 *
 * It should return true if there was no problem processing the event, and
 * false if it was.
 */
using EventReceiver = std::function<bool(const EntityEvent&)>;


/**
 * Some sort of receiver data storage
 */
struct ReceiverData {
    std::string name;
    EventReceiver receiver;
    std::vector<ActionQueueEvent> events;

    ReceiverData(std::string name, EventReceiver r, std::initializer_list<ActionQueueEvent> events)
        : name(name), receiver(r), events(events)
        {}
};

/**
 * Acts like a central hub for events that happen in the game.
 * Receiving events here is good if you want to know when a certain entity was
 * created, or died, or has been attacked.
 */
class ActionQueue
{
private:
    std::queue<EntityEvent> events;
    std::vector<ReceiverData> receivers;

public:
    void addEmitter(EventEmitter* e);
    void addReceiver(std::string name, EventReceiver r, std::initializer_list<ActionQueueEvent> events);

    void removeReceiver(std::string name);
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

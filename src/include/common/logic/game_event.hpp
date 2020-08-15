#pragma once

#include <memory>
#include <queue>
#include <string>

namespace familyline::logic
{
// TODO: rename GameObject to GameEntity?

/**
 * An event
 *
 * Our events will be used to broadcast important events that can
 * work as triggers for the entities who listen to it, like mission
 * completion.
 * It might even be used to reconstruct the game, so that a save is simply just
 * a bunch of events serialized.
 */
enum EventType {
    //! Object has been added to the object manager
    ObjectCreated = 1,

    //! Object has been removed from the object manager
    ObjectDestroyed,

    //! Object position has changed
    ObjectMoved,

    //! Object state has changed
    //! \see ObjectState
    ObjectStateChanged,

    //! Object is attacking other object
    ObjectAttack,
};

class EventEmitter;

class ActionQueue;

/**
 * Possible states of an object
 *
 * It goes like this:
 * (the diagram requires an 80-column monitor to be viewed right
 *
 *   (User requests object creation)
 *                 |
 *                \|/
 *            [ Creating ]
 *                 |
 *                 | Object build finishes
 *            [ Created ]
 *  (1)->------->--|
 *                 |
 *                \|/ Object stopped doing actions
 *              [ Idle ]
 *                /|\-------------------------------->(2)
 / | \      Object has been killed
 /  |  \     (either by the user or other unit)
 *             /   |   \
 *   ----------    |    \-------------------[Damaged]--->-\
 *   |             |       Object has been                | Object lost more
 *   | Object started      attacked                       | than 50% of HP
 *   | working on some            (1)<----[Wounded]----<--/
 *   | task (like mining
 *   | or hunting) |           /------------<----------\
 *  \|/            \------>----\->------>[Attacking]-->/------------------>(1)
 * [ Working ]        Object started an                  Attacked unit
 *   |                attack                             died, or the attacker
 *   |                                                   left
 *   |
 *   |
 *   | Object stopped working
 *  \|/on that work
 *  (1)
 *
 *  (2)-------[Dying]--|-------------------------->---\
 *                     Death animation started        |
 *                                                    |
 *  (Remove the object <----------------[Dead]---|----/
 *   from the manager)              Death animation ended
 */
enum ObjectState {

    /**
     * The object has started being built.
     *
     * Usable only for constructions and certain units
     */
    Creating = 1,

    /**
     * This object exists in the world and is fully build
     */
    Created,

    /**
     * This object has started doing nothing
     */
    Idle,

    //! Unit is working
    Working,

    //! Object is attacked
    Attacking,

    //! Object is badly damaged, like < 50% health
    Damaged,

    //! Object has suffered some damage
    Wounded,

    /**
     * Object has reached 0 HP and started to die.
     *
     * Maybe it is time to run that death animation
     */
    Dying,

    //! Object is dead, time to draw the corpse
    Dead,
};

/**
 * Event
 */
struct Event {
    // Event timestamp, in microseconds.
    long long int timestamp;
    EventType type;

    EventEmitter* emitter;

    // union {
    struct {
        int id;
        std::string name;
        int x, y;
        ObjectState objectState;  // states are Creating, Created, Idle, Working, Attacking,
                                  // Damaged, Wounded (badly damaged), Dying and Dead.
    } object;

    struct {
        int attackerID;
        std::string name;
        int attackedID;

        double damageDealt;
        double attackedHealth;

    } attack;
    //};

    Event() = default;
    Event(EventType);
    Event(const Event&);
    ~Event() {}
};

/**
 * Event receiver
 */
class EventReceiver
{
protected:
    std::queue<Event> events;

public:
    virtual const std::string getName() = 0;

    /**
     * Push the event to the receiver internal queue, so it can be
     * pulled by anyone who is listening through this receiver
     */
    void pushEvent(Event& e);

    bool pollEvent(Event& e);
};

/**
 * Event emitter
 */
class EventEmitter
{
    friend class ActionQueue;

protected:
    ActionQueue* queue;

    /**
     * Pushes the event to the central action queue
     */
    void pushEvent(Event& e);

public:
    virtual const std::string getName() = 0;
};
}  // namespace familyline::logic

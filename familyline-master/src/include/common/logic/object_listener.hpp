#pragma once

/**
 * Object listener
 *
 * Since a lot of classes will need to listen for object actions, like
 * pathfinders (to see if an object moved), object pickers (to see the
 * object position) and others, this class will manage the object
 * listening
 *
 * (C) 2019-2021 Arthur Mendes
 */

/// TODO: refactor EventReceiver to be a callback instead of a full class with inheritance, etc.

#include <set>
#include <functional>

#include <common/logic/game_event.hpp>
#include <common/logic/game_entity.hpp>

namespace familyline::logic
{
class ObjectListener
{
private:

    /// A set of id=>is_alive values
    ///
    /// I use a set so I don't need to worry about filtering
    /// duplicates
    std::set<object_id_t> _objects;

    /**
     * Update the object statuses according to the events
     */
    bool updateObjects(const EntityEvent& e);

public:
    ObjectListener();

    void clear() { _objects.clear(); }
    
    std::set<object_id_t> getAliveObjects() const;

    ~ObjectListener();
};
}  // namespace familyline::logic

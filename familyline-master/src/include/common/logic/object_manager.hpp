#pragma once

#include <memory>
#include <optional>
#include <vector>

#include <common/logic/action_queue.hpp>
#include <common/logic/game_entity.hpp>

/// TODO: add event creation on object add/delete

namespace familyline::logic
{
/**
 * Basic object event emitter
 */
class ObjectEventEmitter : public EventEmitter
{
public:
    ObjectEventEmitter();

    /**
     * Notify the creation start
     *
     * The creation end (when we will send the Created event) will be sent when
     * the object gets rendered for the first time, or when the object gets fully
     * built
     */
    void notifyCreationStart(object_id_t id, const std::string& name);

    /**
     * Notify the removal
     *
     * This is when the object gets fully removed.
     * No more operations with it will be executed.
     */
    void notifyRemoval(object_id_t id, const std::string& name);
};

class ObjectManager
{
private:
    std::vector<std::shared_ptr<GameEntity>> _objects;
    int _lastID                      = 0;
    ObjectEventEmitter* eventEmitter = nullptr;
    
public:
    ObjectManager();

    /**
     * Add an object to the manager.
     *
     * Adding means only getting the object an ID (therefore making
     * it valid) and adding it to the manager, so it can be updated
     * automatically at each game engine iteration
     *
     * Returns the ID
     */
    object_id_t add(std::shared_ptr<GameEntity>&& o);

    /**
     * Removes an object from the manager
     */
    void remove(object_id_t id);

    /**
     * Update every object registered into the manager
     *
     * TODO: update in a certain order?
     */
    void update();

    /**
     * Gets an object from its ID
     *
     * Returns an optional filled with the object if found, or an empty one if
     * not
     */
    std::optional<std::shared_ptr<GameEntity>> get(object_id_t id) const;

    ~ObjectManager();

};
}  // namespace familyline::logic

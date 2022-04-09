/**
 * Object path manager
 * Acts as a higher level interface to the pathfinder
 *
 * Copyright (C) 2021 Arthur Mendes
 */
#pragma once

#include <common/logic/game_event.hpp>
#include <common/logic/game_object.hpp>
#include <common/logic/object_manager.hpp>
#include <common/logic/pathfinder.hpp>
#include <common/logic/terrain.hpp>
#include <common/logic/types.hpp>
#include <deque>
#include <glm/fwd.hpp>
#include <optional>
#include <vector>

namespace familyline::logic
{
using PathHandle = unsigned long int;

/**
 * Manages pathfinding for multiple objects
 */
class ObjectPathManager
{
public:
    ObjectPathManager(const Terrain& t);

    ~ObjectPathManager();

    ObjectPathManager(ObjectPathManager&)       = delete;
    ObjectPathManager(const ObjectPathManager&) = delete;

    /**
     * The object path manager is, more or less, a pathing state machine.
     *
     * So, because of this, every pathed object has a status
     * The statuses are those declared in this enum
     *
     * The state flow is like the graph below:
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     *
     */
    enum class PathState {
        /// The path ref has just been created
        Created,

        /// Pathing in progress.
        /// The pathfinder is running.
        Pathing,

        /// The pathfinder is done
        /// We are only traversing the path.
        Traversing,

        /// We encountered an unexpected obstacle, and we need
        /// to dodge from it
        Dodging,

        /// Our path elements ended, but a path to the destination is still
        /// possible
        ///
        /// It happens when the path positions end before we reach a
        /// destination, but the pathfinder did not fail. This case is reached
        /// when we limit the number of pathfinder iterations to avoid stalling
        /// the machine in a single pathfinder cycle.
        ///
        /// The repathing also happens when you order the entity to move (aka
        /// the mouse right click) while it is following a path.
        Repathing,

        /// We completed the traversing.
        /// We will delete this ref next tick.
        Completed,

        /// The path is impossible to make
        /// (note that we can discover that after starting traversal)
        ImpossiblePath,

        /// We detected the death of an entity (due to a lifecycle event)
        /// It is treated more or less like the completed event
        Died

    };

    /**
     * The associated state of each pathed object
     */
    struct PathRef {
        std::shared_ptr<GameObject> o;

        PathState state = PathState::Created;

        /// The position that it was set in the previous `update()` call.
        /// This will be useful when we introduce the speed factor.
        glm::vec2 previous_position;

        /// The position list set by the pathfinder.
        /// Here, it is actually a queue, so that removing the first element and
        /// adding elements at the end is easier.
        std::deque<glm::vec2> positions;

        /// The pathfinder for this ref.
        std::unique_ptr<Pathfinder> pf;

        /// The final destination of this entity.
        glm::vec2 destination;

        /// How many ticks to wait before proceeding
        int wait = 0;

        /// This pathref is currently repathing to dodge another.
        bool isDodging = false;
    };

    /**
     * A simpler structure, to handle static objects.
     */
    struct StaticRef {
        bool valid = false;
        glm::vec2 position;
        glm::vec2 size;
    };

    std::vector<PathRef> refs_;
    std::vector<object_id_t> to_delete_;

    std::unordered_map<object_id_t, StaticRef> statics_;

    void update(ObjectManager& om);

    void doPathing(std::shared_ptr<GameObject> o, glm::vec2 destination);

    size_t pathCount() const { return refs_.size(); }

    void blockBitmapArea(int x, int y, int w, int h);

private:
    const Terrain& t_;
    std::vector<bool> base_bitmap_;

    EventReceiver obj_events_;

    /**
     * Generate a terrain bitmap with our known pathrefs
     */
    std::vector<bool> generateBitmap(std::vector<object_id_t> exclude_ids = {}) const;

    /**
     * Will this object collide with the currently pathrefs?
     *
     * (The other entities are assured not to collide because of the
     *  bitmap)
     *
     * We return a pointer to the first pathref that will collide
     * with us, or nullptr if no collision is detected
     */
    PathRef* willCollide(
        object_id_t ourid, const std::vector<glm::vec2>& positions, glm::vec2 oursize);

    void updateRefState(PathRef& ref);

    bool handleEntityEvent(const EntityEvent& e);
};

}  // namespace familyline::logic

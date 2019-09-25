#pragma once

/**
 * Renders objects by sending it to the graphics engine
 *
 * It is a "bridge" class between the logic and the graphical engine
 */

#include <map>
#include "object_components.hpp"
#include "game_object.hpp"
#include "terrain.hpp"

namespace familyline::logic {

/**
 * This struct exists because C++ does not lets you create a map
 * with references, because references are, basically, constant pointers
 * (you can change the thing it points to, but you can't change it to 
 * point to something else after you set its value)
 */
    struct RendererSlot {
        object_id_t id;
        LocationComponent& component;

        RendererSlot(object_id_t id, LocationComponent& c)
            : id(id), component(c)
            {}

        RendererSlot operator=(const RendererSlot& v) {
            return RendererSlot(v.id, v.component);
        }
    };

    class ObjectRenderer {
    private:
        std::vector<RendererSlot> components;
        const Terrain& _terrain;

    public:
        ObjectRenderer(const Terrain& t)
            : _terrain(t)
            {}

        void add(GameObject& o);
        void remove(object_id_t id);
        void update();
    };

}

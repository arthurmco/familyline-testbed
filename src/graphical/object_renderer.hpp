#pragma once

/**
 * Renders objects by sending it to the graphics engine
 *
 * It is a "bridge" class between the logic and the graphical engine
 */

#include <vector>
#include <map>
//#include "object_components.hpp"
#include "../logic/GameObject.hpp"
#include "../logic/Terrain.hpp"

typedef int object_id_t;

namespace familyline::graphics {
/**
 * This struct exists because C++ does not lets you create a map
 * with references, because references are, basically, constant pointers
 * (you can change the thing it points to, but you can't change it to 
 * point to something else after you set its value)
 */
    struct RendererSlot {
        object_id_t id;
        familyline::logic::GameObject* component;

        RendererSlot(object_id_t id, GameObject* c)
            : id(id), component(c)
            {}
    };

    class ObjectRenderer {
    private:
        std::vector<RendererSlot> components;
        const familyline::logic::Terrain& _terrain;

    public:
        ObjectRenderer(const familyline::logic::Terrain& t)
            : _terrain(t)
            {}

        void add(familyline::logic::GameObject* const o);
        void remove(object_id_t id);
        void update();
    };
}

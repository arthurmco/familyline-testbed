/***
    Object drawing class

    Copyright (C) 2016 Arthur M

***/
#include <list>
#include <vector>

#include "LocatableObject.hpp"
#include "ObjectManager.hpp"

#include "../graphical/TerrainRenderer.hpp"
#include "../graphical/SceneManager.hpp"

#ifndef OBJECTRENDERER_HPP
#define OBJECTRENDERER_HPP

namespace Tribalia {
namespace Logic {

    class ObjectRenderer
    {
    private:
        ObjectManager* _om;
        Tribalia::Graphics::SceneManager* _sm;

        std::list<LocatableObject*> _objects;
        std::vector<int> _IDs;

    public:
        ObjectRenderer(ObjectManager*, Tribalia::Graphics::SceneManager*);

        /* Check for new objects, add them to the list */
        void Check();

        /* Update object meshes */
        void Update();

        /* Check if the eye-space ray collides with any rendered object.
            Returns the object, or nullptr if any.
            Also return the collided world-space coords on world_pos vec3. */
        LocatableObject* CheckRayCollide(glm::vec3 eye_ray, glm::vec3* world_pos) {return nullptr;}

    };

}
} /* Tribalia */


#endif /* end of include guard: OBJECTRENDERER_HPP */

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

    struct ObjectRenderData {
        int ID;
        Tribalia::Graphics::Mesh* m;
        bool ok;
    };

    class ObjectRenderer
    {
    private:
        ObjectManager* _om;
        Tribalia::Graphics::SceneManager* _sm;

        std::list<LocatableObject*> _objects;
        std::vector<ObjectRenderData> _IDs;

    public:
        ObjectRenderer(ObjectManager*, Tribalia::Graphics::SceneManager*);

        /*	Check for new and deleted objects, add them to the list 
			Return true if we have object changes, false if we haven't
		*/
        bool Check();

        /* Update object meshes */
        void Update();

        /* Check if the worldobjects-space ray collides with any rendered object.
            Returns the object, or nullptr if any.
            Also return the collided world-space coords on world_pos vec3. */
        LocatableObject* CheckRayCollide(glm::vec3 eye_ray, glm::vec3* world_pos);

        /* Check the terrain position that the cursor is projected at, in
            OpenGL coordinates */
        glm::vec3 CheckTerrainPositions(glm::vec2 positions, int winwidth, int winheight);
    };

}
} /* Tribalia */


#endif /* end of include guard: OBJECTRENDERER_HPP */

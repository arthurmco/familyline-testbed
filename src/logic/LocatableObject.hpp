
/***
    Locatable object implementation

    Copyright 2016 Arthur M.

***/

#include "GameObject.hpp"
#include "../graphical/Mesh.hpp"

#ifndef LOCATABLEOBJECT_HPP
#define LOCATABLEOBJECT_HPP

namespace Tribalia {
    namespace Logic {

        /* A locatable object is a object that have a space position
            (pretty much every single one) */

        class LocatableObject : public GameObject {
        public:
            LocatableObject(int oid, int tid, const char* name,
                float xPos = -1, float yPos = -1, float zPos = -1);

            /* Get radius */
            float GetRadius();

            /* Get the rotation, in radians */
            float GetRotation(void);

            void SetMesh(Tribalia::Graphics::Mesh*);
            Tribalia::Graphics::Mesh* GetMesh();
        };

    }
}

#endif

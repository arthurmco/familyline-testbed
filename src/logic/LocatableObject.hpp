
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
            LocatableObject(int oid, int tid, const char* name);
            LocatableObject(int oid, int tid, const char* name,
                float xPos, float yPos, float zPos);

            void SetX(float); float GetX();
            void SetY(float); float GetY();
            void SetZ(float); float GetZ();

            void SetMesh(Tribalia::Graphics::Mesh*);
            Tribalia::Graphics::Mesh* GetMesh();

        };

    }
}

#endif

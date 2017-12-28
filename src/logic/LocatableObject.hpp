
/***
    Locatable object implementation

    Copyright 2016, 2017 Arthur M.

***/

#include <memory>

#include "GameObject.hpp"
#include "../graphical/Mesh.hpp"

#include "IMesh.hpp"

#ifndef LOCATABLEOBJECT_HPP
#define LOCATABLEOBJECT_HPP

namespace Tribalia {
    namespace Logic {

        /* A locatable object is a object that have a space position
            (pretty much every single one) */

        class LocatableObject : public GameObject {
	private:
	    /* A visible representation of the object

	       This is stored in a abstract class, because the server app will
	       use this namespace, and the server has no graphics.
	    */ 
	    std::shared_ptr<IMesh> _mesh;

        public:
            LocatableObject(int oid, int tid, const char* name,
                float xPos = -1, float yPos = -1, float zPos = -1);

            /* Get radius */
            float GetRadius();

            /* Get the rotation, in radians */
            float GetRotation(void);

            void SetMesh(IMesh*);
            IMesh* GetMesh();
        };

    }
}

#endif

#include "LocatableObject.hpp"

using namespace Tribalia::Logic;

LocatableObject::LocatableObject(int oid, int tid, const char* name) :
    GameObject(oid, tid, name, -1, -1, -1)
    {
        DEF_PROPERTY("rotation",-1);
    }
LocatableObject::LocatableObject(int oid, int tid, const char* name,
    float xPos, float yPos, float zPos) :
    GameObject(oid, tid, name, xPos, yPos, zPos)
    {
        DEF_PROPERTY("rotation",-1);

        DEF_PROPERTY("mesh", (Tribalia::Graphics::Mesh*)nullptr);
    }

float LocatableObject::GetRotation() { return GET_PROPERTY(float, "rotation"); }

/* Get radius from mesh data. */
float LocatableObject::GetRadius()
{
    Tribalia::Graphics::Mesh* m = GET_PROPERTY(Tribalia::Graphics::Mesh*, "mesh");
    if (m) {

	if (_radius < 0.0f) {
	
	    /*  Multiply by 4, the default number for gl to game coord 
		conversion.
		TODO: Use Terrain::GraphicalToGame() ?
		Divide by 2, the diameter to radius converter */
	    _radius = (m->GetBoundingBox().maxX - m->GetBoundingBox().minX) * (4/2);
	}

	return _radius;
	
    }
    return 0.0f;
}

void LocatableObject::SetMesh(Tribalia::Graphics::Mesh* m)
{
    SET_PROPERTY("mesh", m);
    _radius = -1.0f; //force radius recalculation
}
Tribalia::Graphics::Mesh* LocatableObject::GetMesh()
{
    return GET_PROPERTY(Tribalia::Graphics::Mesh*, "mesh");
}

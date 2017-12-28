#include "LocatableObject.hpp"

using namespace Tribalia::Logic;

LocatableObject::LocatableObject(int oid, int tid, const char* name,
    float xPos, float yPos, float zPos) :
    GameObject(oid, tid, name, xPos, yPos, zPos)
    {
        DEF_PROPERTY("rotation",-1);
    }

float LocatableObject::GetRotation() { return GET_PROPERTY(float, "rotation"); }

/* Get radius from mesh data. */
float LocatableObject::GetRadius()
{
    Tribalia::Graphics::Mesh* m = ((Tribalia::Graphics::Mesh*)_mesh.get());
    if (m) {

	if (_radius < 0.0f) {
	    m->GenerateBoundingBox();
	
	    /*  Multiply by 4, the default number for gl to game coord 
		conversion.
		TODO: Use Terrain::GraphicalToGame() ?
		Divide by 2, the diameter to radius converter */
	    _radius = (m->GetBoundingBox().maxX - m->GetBoundingBox().minX) * (4/2);
	}

	return _radius;
	
    }
    return (_radius < 0.0f) ? 0.0f : _radius;
}

void LocatableObject::SetMesh(IMesh* m)
{
    _mesh = std::shared_ptr<IMesh>(m);
    _radius = -1.0f; //force radius recalculation
}
IMesh* LocatableObject::GetMesh()
{
    return _mesh.get();
}

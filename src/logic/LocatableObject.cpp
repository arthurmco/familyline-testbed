#include "LocatableObject.hpp"

using namespace Tribalia::Logic;

LocatableObject::LocatableObject(int oid, int tid, const char* name) :
    GameObject(oid, tid, name)
    {
       _xPos = -1;
       _yPos = -1;
       _zPos = -1;
        DEF_PROPERTY("rotation",-1);
    }
LocatableObject::LocatableObject(int oid, int tid, const char* name,
    float xPos, float yPos, float zPos) :
        GameObject(oid, tid, name)
    {
        _xPos = xPos;
        _yPos = yPos;
        _zPos = zPos;
        DEF_PROPERTY("rotation",-1);

        DEF_PROPERTY("mesh", (Tribalia::Graphics::Mesh*)nullptr);
    }

void LocatableObject::SetX(float v) { _xPos = v;  }
float LocatableObject::GetX() { return _xPos; }

void LocatableObject::SetY(float v) { _yPos = v; }
float LocatableObject::GetY() { return _yPos; }

void LocatableObject::SetZ(float v) { _zPos = v; }
float LocatableObject::GetZ() { return _zPos; }

float LocatableObject::GetRotation() { return GET_PROPERTY(float, "rotation"); }

/* Get radius from mesh data. */
float LocatableObject::GetRadius()
{
    Tribalia::Graphics::Mesh* m = GET_PROPERTY(Tribalia::Graphics::Mesh*, "mesh");
    if (m) {
        /*  Multiply by 4, the default number for gl to game coord conversion.
            Divide by 2, the diameter to radius converter */
        return (m->GetBoundingBox().maxX - m->GetBoundingBox().minX) * (4/2);
    }
    return 0.0f;
}

void LocatableObject::SetMesh(Tribalia::Graphics::Mesh* m)
{
    SET_PROPERTY("mesh", m);
}
Tribalia::Graphics::Mesh* LocatableObject::GetMesh()
{
    return GET_PROPERTY(Tribalia::Graphics::Mesh*, "mesh");
}

#include "LocatableObject.hpp"

using namespace Tribalia::Logic;

LocatableObject::LocatableObject(int oid, int tid, const char* name) :
    GameObject(oid, tid, name)
    {
        DEF_PROPERTY("xPos",-1);
        DEF_PROPERTY("yPos",-1);
        DEF_PROPERTY("zPos",-1);
        DEF_PROPERTY("rotation",-1);
    }
LocatableObject::LocatableObject(int oid, int tid, const char* name,
    float xPos, float yPos, float zPos) :
        GameObject(oid, tid, name)
    {
        DEF_PROPERTY("xPos", xPos);
        DEF_PROPERTY("yPos", yPos);
        DEF_PROPERTY("zPos", zPos);
        DEF_PROPERTY("rotation",-1);

        DEF_PROPERTY("mesh", (Tribalia::Graphics::Mesh*)nullptr);
    }

void LocatableObject::SetX(float v) { SET_PROPERTY("xPos", v);  }
float LocatableObject::GetX() { return GET_PROPERTY(float, "xPos"); }

void LocatableObject::SetY(float v) { SET_PROPERTY("yPos", v); }
float LocatableObject::GetY() { return GET_PROPERTY(float, "yPos"); }

void LocatableObject::SetZ(float v) { SET_PROPERTY("zPos", v); }
float LocatableObject::GetZ() { return GET_PROPERTY(float, "zPos"); }

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

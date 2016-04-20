

#include "LocatableObject.hpp"

using namespace Tribalia::Logic;

LocatableObject::LocatableObject(int oid, int tid, const char* name) :
    GameObject(oid, tid, name)
    {
        this->AddProperty("xPos",-1);
        this->AddProperty("yPos",-1);
        this->AddProperty("zPos",-1);

    }
LocatableObject::LocatableObject(int oid, int tid, const char* name,
    float xPos, float yPos, float zPos) :
        GameObject(oid, tid, name)
    {
        this->AddProperty("xPos", xPos);
        this->AddProperty("yPos", yPos);
        this->AddProperty("zPos", zPos);

    }

void LocatableObject::SetX(float v) { this->SetProperty("xPos", v);  }
float LocatableObject::GetX() { return this->GetProperty<float>("xPos"); }

void LocatableObject::SetY(float v) { this->SetProperty("yPos", v); }
float LocatableObject::GetY() { return this->GetProperty<float>("yPos"); }

void LocatableObject::SetZ(float v) { this->SetProperty("zPos", v); }
float LocatableObject::GetZ() { return this->GetProperty<float>("zPos"); }



#include "LocatableObject.hpp"

using namespace Tribalia::Logic;

LocatableObject::LocatableObject(int oid, int tid, const char* name) :
    GameObject(oid, tid, name)
    {
        this->AddProperty("xPos",-1);
        this->AddProperty("yPos",-1);
        this->AddProperty("zPos",-1);
        this->AddProperty("rotation",-1);
    }
LocatableObject::LocatableObject(int oid, int tid, const char* name,
    float xPos, float yPos, float zPos) :
        GameObject(oid, tid, name)
    {
        this->AddProperty("xPos", xPos);
        this->AddProperty("yPos", yPos);
        this->AddProperty("zPos", zPos);
        this->AddProperty("rotation",-1);
        
        this->AddProperty("mesh", (Tribalia::Graphics::Mesh*)nullptr);
    }

void LocatableObject::SetX(float v) { this->SetProperty("xPos", v);  }
float LocatableObject::GetX() { return this->GetProperty<float>("xPos"); }

void LocatableObject::SetY(float v) { this->SetProperty("yPos", v); }
float LocatableObject::GetY() { return this->GetProperty<float>("yPos"); }

void LocatableObject::SetZ(float v) { this->SetProperty("zPos", v); }
float LocatableObject::GetZ() { return this->GetProperty<float>("zPos"); }

float LocatableObject::GetRotation() { return this->GetProperty<float>("rotation"); }

void LocatableObject::SetMesh(Tribalia::Graphics::Mesh* m)
{
    this->SetProperty("mesh", m);
}
Tribalia::Graphics::Mesh* LocatableObject::GetMesh()
{
    return this->GetProperty<Tribalia::Graphics::Mesh*>("mesh");
}

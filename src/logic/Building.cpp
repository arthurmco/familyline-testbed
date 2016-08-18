#include "Building.hpp"

using namespace Tribalia::Logic;


Building::Building(int oid, int tid, const char* name)
    : AttackableObject(oid, tid, name)
    {
        this->AddProperty("buildMaterial", 0.0f);
        this->AddProperty("buildingStrength", 1.0f);
    }

Building::Building(int oid, int tid, const char* name,
    float x, float y, float z)
    : AttackableObject(oid, tid, name, x, y, z)
    {
        this->AddProperty("buildMaterial", 0.0f);
        this->AddProperty("buildingStrength", 1.0f);
		InitGarrisoning();
    }

Building::Building(int oid, int tid, const char* name,
    float x, float y, float z, int maxHP,
    float baseAtk, float baseArmor)
    : AttackableObject(oid, tid, name, x, y, z, maxHP, baseAtk, baseArmor)
    {
        this->AddProperty("buildMaterial", 0.0f);
        this->AddProperty("buildingStrength", 1.0f);
		InitGarrisoning();
    }
Building::Building(int oid, int tid, const char* name,
    float x, float y, float z, int maxHP,
    float baseAtk, float baseArmor,
    float buildMaterial, float buildingStrength, int garrisonCapacity)
    : AttackableObject(oid, tid, name, x, y, z, maxHP, baseAtk, baseArmor)
    {
        this->AddProperty("buildMaterial", buildMaterial);
        this->AddProperty("buildingStrength", buildingStrength);
		InitGarrisoning(garrisonCapacity);
    }

float Building::GetBuildMaterial()
{
    return this->GetProperty<float>("buildMaterial");

}

void Building::SetBuildMaterial(float bm)
{
    this->SetProperty("buildMaterial", bm);
}

float Building::GetBuildingStrength()
{
    return this->GetProperty<float>("buildingStrength");

}

void Building::SetBuildingStrength(float bs)
{
    this->SetProperty("buildingStrength", bs);
}

inline void Building::InitGarrisoning(int capacity)
{
	this->_maximumCapacityUnits = capacity;
	this->_garrisonedCount = 0;
	if (capacity > 0) {
		this->_garrisonedUnits = new Unit*[capacity];		
	}
}

int Building::GetMaximumGarrisonCapacity()
{
	return _maximumCapacityUnits;
}
int Building::GetGarrisonedUnitCound()
{
	return _garrisonedCount;
}
#include "Building.hpp"

using namespace Tribalia::Logic;



Building::Building(int oid, int tid, const char* name,
		   float x, float y, float z, int maxHP, float HP,
		   float baseAtk, float baseArmor,
		   float buildMaterial, float buildingStrength,
		   int garrisonCapacity)
    : AttackableObject(oid, tid, name, x, y, z, maxHP, HP, baseAtk, baseArmor)
    {
        DEF_PROPERTY("buildMaterial", buildMaterial);
        DEF_PROPERTY("buildingStrength", buildingStrength);
		DEF_PROPERTY("birthPointX", GetX());
        DEF_PROPERTY("birthPointY", GetZ());
        InitGarrisoning(garrisonCapacity);
    }

float Building::GetBuildMaterial()
{
    return GET_PROPERTY(float,"buildMaterial");

}

void Building::SetBuildMaterial(float bm)
{
    SET_PROPERTY("buildMaterial", bm);
}

float Building::GetBuildingStrength()
{
    return GET_PROPERTY(float,"buildingStrength");

}

void Building::SetBuildingStrength(float bs)
{
    SET_PROPERTY("buildingStrength", bs);
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

void Building::GetUnitBirthPoint(float& x, float& y)
{
    x = GET_PROPERTY(float, "birthPointX");
    y = GET_PROPERTY(float, "birthPointY");
}
void Building::SetUnitBirthPoint(float x, float y)
{
    SET_PROPERTY("birthPointX", x);
    SET_PROPERTY("birthPointY", y);
}

void Building::Train(Unit* unit) 
{
    float x, y;
    this->GetUnitBirthPoint(x, y);
    unit->SetX(x);
    unit->SetZ(y);
}


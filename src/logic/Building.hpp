
/***
    Building base class

    Copyright 2016 Arthur M.

***/

#include "AttackableObject.hpp"

#ifndef BUILDING_H
#define BUILDING_H

namespace Tribalia {
namespace Logic {

    class Building : public AttackableObject
    {
    public:
        Building(int oid, int tid, const char* name);
        Building(int oid, int tid, const char* name,
            float x, float y, float z);
        Building(int oid, int tid, const char* name,
            float x, float y, float z, int maxHP,
            float baseAtk, float baseArmor);
        Building(int oid, int tid, const char* name,
            float x, float y, float z, int maxHP,
            float baseAtk, float baseArmor,
            float buildMaterial, float BuildingStrength);

        float GetBuildMaterial(); void SetBuildMaterial(float);
        float GetBuildingStrength(); void SetBuildingStrength(float);

    };

}
}


#endif /* end of include guard: BUILDING_H */


/***
    Building base class

    Copyright 2016 Arthur M.

***/

#include "AttackableObject.hpp"
#include "Unit.hpp"

#ifndef BUILDING_H
#define BUILDING_H

namespace Tribalia {
namespace Logic {

    class Building : public AttackableObject
    {
	private:
		inline void InitGarrisoning(int capacity = 0);

	protected:
		/*	The maximum amount of people that can be garrisoned inside the 
			building */
		int _maximumCapacityUnits;

		Unit** _garrisonedUnits;
		int _garrisonedCount;

        /*  Train an unit. 
            It will appear instantly, so it's your responsability
            to define a delay */
        void Train(Unit* unit);

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
            float buildMaterial, float BuildingStrength,
			int garrisonCapacity);

        float GetBuildMaterial(); void SetBuildMaterial(float);
        float GetBuildingStrength(); void SetBuildingStrength(float);

		int GetMaximumGarrisonCapacity();
		int GetGarrisonedUnitCound();

		/* Get/set where your units will be created or degarrisoned, in game units */
		void GetUnitBirthPoint(float& x, float& y);
		void SetUnitBirthPoint(float x, float y);
    };

}
}


#endif /* end of include guard: BUILDING_H */


/***
	Building base class

	Copyright 2016 Arthur Mendes.

***/

#ifndef BUILDING_H
#define BUILDING_H

#include "AttackableObject.hpp"
#include "Unit.hpp"
#include <list>

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
			std::list<Unit*> _garrisonedUnits;
			int _garrisonedCount;

			/*  Train an unit.
				It will appear instantly, so it's your responsability
				to define a delay */
			void Train(Unit* unit);

		public:
			Building(int oid, int tid, const char* name,
				float x = -1, float y = -1, float z = -1,
				int maxHP = 1, float HP = -1, float baseAtk = 0.0,
				float baseArmor = 0.0, float buildMaterial = 0.0f,
				float BuildingStrength = 1.0f, int garrisonCapacity = 0);
			Building() : Building(-1, -1, "") {}

			float GetBuildMaterial(); void SetBuildMaterial(float);
			float GetBuildingStrength(); void SetBuildingStrength(float);

			int GetMaximumGarrisonCapacity();
			int GetGarrisonedUnitCound();

			/* Get/set where your units will be created or degarrisoned, in game units */
			void GetUnitBirthPoint(float& x, float& y);
			void SetUnitBirthPoint(float x, float y);

			virtual ~Building() {}

		};

	}
}


#endif /* end of include guard: BUILDING_H */


/***
    Attackable object class

    Copyright 2016 Arthur M.

***/

#include "LocatableObject.hpp"

#include <algorithm>

#ifndef ATTACKABLEOBJECT_HPP
#define ATTACKABLEOBJECT_HPP

namespace Tribalia {
    namespace Logic {

        /* This class represents an object that can attack... and be attacked */

        class AttackableObject : public LocatableObject {
        public:
            AttackableObject(int oid, int tid, const char* name);
            AttackableObject(int oid, int tid, const char* name,
                float x, float y, float z);
            AttackableObject(int oid, int tid, const char* name,
                float x, float y, float z, int maxHP,
                float baseAtk, float baseArmor);
			
            int GetMaxHP(void);
            float GetHP(void);
            float GetBaseAttack(void);
            float GetBaseArmor(void);

            float SetBaseAttack(float);
            float SetBaseArmor(float);

            float GetUnitBonus(void);
            float GetBasicBuildingBonus(void);
            float GetMediumBuildingBonus(void);
            float GetAdvancedBuildingBonus(void);

        protected:
            /* Increase HP until maximum */
            float Heal(float);

            /* Decrease HP until minimum */
            float Damage(float);

        };

    }
}


#endif /* end of include guard: ATTACKABLEOBJECT_HPP */

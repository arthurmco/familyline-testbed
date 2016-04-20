
/***
    Attackable object class

    Copyright 2016 Arthur M.

***/

#include "LocatableObject.hpp"

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


            int GetMaxHP();
            float GetHP();
            float GetBaseAttack();
            float GetBaseArmor();

            float SetBaseAttack(float);
            float SetBaseArmor(float);            
        protected:
            /* Increase HP until maximum */
            float Heal(float);

            /* Decrease HP until minimum */
            float Damage(float);

        };

    }
}


#endif /* end of include guard: ATTACKABLEOBJECT_HPP */

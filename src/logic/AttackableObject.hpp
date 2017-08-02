
/***
    Attackable object class

    Copyright 2016, 2017 Arthur M.

***/

#include "LocatableObject.hpp"
#include "Action.hpp"

#include <cstring>
#include <cstdlib> //srand(), rand()
#include <algorithm>

#ifndef ATTACKABLEOBJECT_HPP
#define ATTACKABLEOBJECT_HPP

namespace Tribalia {
    namespace Logic {

	/* The status of attacked or attackable objects */
	enum AttackableStatus {
	    AST_CREATING,
	    AST_ALIVE,
	    AST_DEAD,
	    AST_INVULNERABLE,
	};

        /* This class represents an object that can attack... and be attacked */

        class AttackableObject : public LocatableObject {
        public:
            AttackableObject(int oid, int tid, const char* name,
		float x = -1, float y = -1, float z = -1, int maxHP = 1,
		float HP = -1, float baseAtk = 0.0, float baseArmor = 0.0);

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
	    
	    AttackableStatus GetStatus(void);

	    /* Set the object status. 
	     * Note that you can't set an object to dead */
	    void SetStatus(AttackableStatus);
	    
            /* Retrieve this unit experience
               The experience is tightly associated with the soldier
               strength progression and hit chance */
            int GetExperience();

           
            /* Hit another entity 
               Returns the amount of damage dealt
	       tick is the tick time, in seconds
            */
            float Hit(AttackableObject* other, double tick);

	    /* Check if the other object is within attack range */
	    bool CheckAttackRange(AttackableObject* other);

	    /* Check if other object can be attacked.
	       The other object can be, for example, an ally
	       In this case, makes no sense to attack him */	       
	    bool CheckIfAttackable(AttackableObject* other) { return true; }


        protected:

            /* Increase HP until maximum */
            float Heal(float);

            /* Decrease HP until minimum */
            float Damage(float);

        };

    }
}


#endif /* end of include guard: ATTACKABLEOBJECT_HPP */

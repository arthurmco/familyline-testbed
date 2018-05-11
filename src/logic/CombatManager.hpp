/* Manages the attacks of the objects
   Guarantee that it only attacks objects in its range, and that only the
   exit of range or death stop the attacks


   Copyright (C) 2017 Arthur M
*/

#include <list>
#include <functional>

#include "AttackableObject.hpp"

#ifndef COMBAT_MANAGER
#define COMBAT_MANAGER

namespace Familyline {
namespace Logic {

    struct Combat {
	AttackableObject* attacker;
	AttackableObject* defender;
	
	bool isSuspended = false;
    };

    typedef std::function<void(AttackableObject*)> OnDeathFunction;

    class CombatManager {
    private:
	std::list<Combat> _combats;
	std::list<AttackableObject*> _deads;

	OnDeathFunction _deathfunc = nullptr;

    public:
	void DoAttacks(double tick);

	
	static CombatManager* GetInstance() {
	    static CombatManager* i = nullptr;

	    if (!i)
		i = new CombatManager;

	    return i;
	}

	void AddAttack(AttackableObject* attacker, AttackableObject* defender);

	void SuspendAttack(AttackableObject* attacker);

	/* Set function to be run on the object death */
	void SetOnDeath(OnDeathFunction f);

    };    
}
}


#endif

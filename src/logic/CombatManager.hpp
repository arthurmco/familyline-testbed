/* Manages the attacks of the objects
   Guarantee that it only attacks objects in its range, and that only the
   exit of range or death stop the attacks


   Copyright (C) 2017 Arthur M
*/

#include <list>
#include <functional>
#include <memory>

#include "AttackableObject.hpp"

#ifndef COMBAT_MANAGER
#define COMBAT_MANAGER

namespace familyline::logic {

    struct Combat {
	std::weak_ptr<AttackableObject> attacker;
	std::weak_ptr<AttackableObject> defender;
	
	bool isSuspended = false;
    };

    typedef std::function<void(AttackableObject*)> OnDeathFunction;

    class CombatManager {
    private:
	std::list<Combat> _combats;
	std::list<std::weak_ptr<AttackableObject>> _deads;

	OnDeathFunction _deathfunc = nullptr;

	static CombatManager* defaulti;
    public:
	void DoAttacks(double tick);


	static CombatManager* getDefault() { return defaulti; }
	static void setDefault(CombatManager* c) { defaulti = c; }

	void AddAttack(std::weak_ptr<AttackableObject> attacker,
		       std::weak_ptr<AttackableObject> defender);

	void SuspendAttack(std::weak_ptr<AttackableObject> attacker);

	/* Set function to be run on the object death */
	void SetOnDeath(OnDeathFunction f);

    };    
}

#endif

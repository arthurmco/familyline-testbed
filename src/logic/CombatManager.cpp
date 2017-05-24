#include "CombatManager.hpp"

using namespace Tribalia::Logic;

void CombatManager::DoAttacks(double tick)
{
    if (_combats.size() == 0) return;
    
    std::vector<decltype(_combats.begin())> it_removals;
    
    /* Since we're going to save and remove iterators, we're going to use
       the classical for loop */
    for (auto it = _combats.begin(); it != _combats.end(); it++) {
	if (!it->attacker->CheckAttackRange(it->defender) ||
	    it->isSuspended) {
	    // Not in range
	    it_removals.push_back(it);
	    continue;
	}
	
	float f = it->attacker->Hit(it->defender, tick);
	printf("%s dealt %.3f dmg on %s\n", it->attacker->GetName(), f,
			   it->defender->GetName());

	if (it->defender->GetStatus() == AST_DEAD) {
	    it_removals.push_back(it);
	    printf("%s is dead", it->defender->GetName());
	}	
    }

    /* Remove the ones marked for removal */
    for (auto it = it_removals.begin(); it != it_removals.end(); it++) {
	if (_deathfunc)
	    _deathfunc((*it)->defender);

	_combats.erase(*it);
    }

    printf("%d\n", _combats.size());
}

void CombatManager::AddAttack(AttackableObject* attacker,
			      AttackableObject* defender)
{
    Combat c;
    c.attacker = attacker;
    c.defender = defender;

    _combats.push_back(c);
}

void CombatManager::SuspendAttack(AttackableObject* attacker)
{
    /* Mark attacker as suspended */
    for (auto& c : _combats) {
	if (c.attacker->GetObjectID() == attacker->GetObjectID()) {
	    c.isSuspended = true;
	}
    }
}

void CombatManager::SetOnDeath(OnDeathFunction f)
{
    this->_deathfunc = f;
}

#include "CombatManager.hpp"

using namespace Familyline::Logic;

void CombatManager::DoAttacks(double tick)
{
    if (_combats.size() == 0) return;
    

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
	if (c.attacker->getID() == attacker->getID()) {
	    c.isSuspended = true;
	}
    }
}

void CombatManager::SetOnDeath(OnDeathFunction f)
{
    this->_deathfunc = f;
}

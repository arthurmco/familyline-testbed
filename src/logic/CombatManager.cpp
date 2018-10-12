#include "CombatManager.hpp"
#include "ObjectEventEmitter.hpp"
#include "ObjectManager.hpp"

using namespace Familyline::Logic;

CombatManager* CombatManager::defaulti;


void CombatManager::DoAttacks(double tick)
{
    std::vector<decltype(_combats.begin())> delete_iters;
    
    if (_combats.size() == 0) return;
    for (auto it = _combats.begin(); it != _combats.end(); ++it) {
	auto atk = it->attacker.lock();
	auto def = it->defender.lock();
	
	const auto dmg = atk->doAttack(def.get());

	if (!dmg) {
	    /* If off range, schedule it for removal from the combat manager */
	    delete_iters.push_back(it);
	    continue;
	}
	
	
	def->doDamage(dmg.value());

	/* If it was dying, set it to dead and remove both from the combat manager*/
	if (def->getState() == ObjectState::Dying) {
	    auto def_olds = def->setState(ObjectState::Dead);
	    ObjectEventEmitter::pushMessage(ObjectManager::getDefault(),
					    ObjectEvent(def, def_olds, ObjectState::Dead));
	    if (this->_deathfunc)
		this->_deathfunc(def.get());

	    delete_iters.push_back(it);
	    continue;
	}

	/* If the HP is 0, set it to dying */
	if (def->getCurrentLifePoints() <= 0) {
	    auto def_olds = def->setState(ObjectState::Dying);
	    ObjectEventEmitter::pushMessage(ObjectManager::getDefault(),
					    ObjectEvent(def, def_olds, ObjectState::Dying));
	    continue;
	}

	
	/* The attacker will be set to Created event, and the defender will be set to Hurt */
	auto atk_olds = atk->setState(ObjectState::Created);
	auto def_olds = def->setState(ObjectState::Hurt);

	if (atk_olds != ObjectState::Created)
	    ObjectEventEmitter::pushMessage(ObjectManager::getDefault(),
					    ObjectEvent(atk, atk_olds, ObjectState::Created)); 

	if (def_olds != ObjectState::Hurt)
	    ObjectEventEmitter::pushMessage(ObjectManager::getDefault(),
					    ObjectEvent(def, def_olds, ObjectState::Hurt));	
    }

    for (auto& delit : delete_iters) {
	_combats.erase(delit);
    }

}

void CombatManager::AddAttack(std::weak_ptr<AttackableObject> attacker,
			      std::weak_ptr<AttackableObject> defender)
{
    Combat c;
    c.attacker = attacker;
    c.defender = defender;

    _combats.push_back(c);
}

void CombatManager::SuspendAttack(std::weak_ptr<AttackableObject> attacker)
{
    auto shared_atk = attacker.lock();
    
    /* Mark attacker as suspended */
    for (auto& c : _combats) {
	if (c.attacker.lock()->getID() == shared_atk->getID()) {
	    c.isSuspended = true;
	}
    }
}

void CombatManager::SetOnDeath(OnDeathFunction f)
{
    this->_deathfunc = f;
}

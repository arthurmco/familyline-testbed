#include <algorithm>

#include "attack_manager.hpp"
#include "game_object.hpp"
#include "logic_service.hpp"

using namespace familyline::logic;

AttackManagerEventReceiver::AttackManagerEventReceiver()
{
	LogicService::getActionQueue()->addReceiver((EventReceiver*)this, { EventType::ObjectDestroyed });
}

const std::string AttackManagerEventReceiver::getName() { return _name; }

AttackManager::AttackManager()
{
	ame_receiver = new AttackManagerEventReceiver{};
	ame_emitter = new AttackManagerEventEmitter{};
}

std::optional<double> AttackManager::attack(AttackComponent& defender)
{
	return std::optional<double>();
}

void AttackManager::doRegister(int oid, AttackComponent& atk)
{
	this->components[oid] = &atk;
}

attack_handle_t AttackManager::startAttack(int attackerOID, int defenderOID)
{
	auto atkhandle = make_attack_handle(attackerOID, defenderOID);

	AttackData adata;
	adata.atk = this->components[attackerOID];
	adata.def = this->components[defenderOID];

	this->attacks[atkhandle] = adata;
	
	return atkhandle;
}

void AttackManager::endAttack(attack_handle_t ahandle)
{
	this->attacks.erase(ahandle);
}


/**
 * Check if any object has been deleted and should be removed
 */
void AttackManager::checkRemovedObjects()
{
	Event e;

	std::vector<std::tuple<attack_handle_t, int>> to_remove;
	while (this->ame_receiver->pollEvent(e)) {
		// The event types are guaranteed to be only the object destruction events
		auto eid = e.object.id;

		for (auto [ahandle, adata] : this->attacks) {
			int atkID, defID;
			std::tie(atkID, defID) = break_attack_handle(ahandle);

			if (eid == atkID || eid == defID) {
				to_remove.push_back(std::make_tuple(ahandle, eid));
			}

		}

		for (auto [cid, _unused] : this->components) {

			bool is_removing = std::find_if(to_remove.begin(), to_remove.end(), [eid](auto a) {
				return std::get<1>(a) == eid;
			}) == to_remove.end();

			if (cid == eid && is_removing) {
				to_remove.push_back(std::make_tuple(0, eid));
			}
		}
	}

	for (auto [handle, oid] : to_remove) {
		printf("removed %x %lx\n", oid, handle);
		if (handle > 0)
			this->attacks.erase(handle);
		
		this->components.erase(oid);
	}
}


void AttackManager::processAttacks()
{
	this->checkRemovedObjects();

	std::vector<attack_handle_t> to_remove;
	for ( auto [ahandle, adata] : this->attacks) {
		auto dmg = adata.atk->doDirectAttack(*adata.def);
		if (!dmg) {
			printf("out of range\n");
			to_remove.push_back(ahandle);
			continue;
		}
		
		auto vdmg = dmg.value_or(0);
		adata.def->object->addHealth(-vdmg);
		this->ame_emitter->generateAttackEvent(adata.atk, adata.def, vdmg);
	}

}

attack_handle_t make_attack_handle(int attackerOID, int defenderOID)
{
	static_assert(sizeof(int) * 2 == sizeof(attack_handle_t), "Mismatched size for attack handle");

	return defenderOID | (((attack_handle_t)attackerOID) << 32);
}

/**
 * Split an attack handle to the attacker and defender
 */
std::tuple<int, int> break_attack_handle(attack_handle_t handle)
{
	return std::make_tuple<int, int>(handle >> 32, handle & 0xffffffff);
}

AttackManagerEventEmitter::AttackManagerEventEmitter()
{
	LogicService::getActionQueue()->addEmitter((EventEmitter*)this);
}

const std::string AttackManagerEventEmitter::getName() { return _name; }


void AttackManagerEventEmitter::generateAttackEvent(AttackComponent* atk, AttackComponent* def, double dmg)
{
	Event eatk(EventType::ObjectAttack);
	Event edef(EventType::ObjectStateChanged);

	eatk.object.objectState = ObjectState::Attacking;
	edef.object.objectState = ObjectState::Wounded;

	auto atkpos = atk->object->getPosition();
	auto defpos = def->object->getPosition();

	eatk.object.id = atk->object->getID();
	eatk.object.name = atk->object->getName();
	eatk.object.x = int(atkpos.x);
	eatk.object.y = int(atkpos.z);

	edef.object.id = def->object->getID();
	edef.object.name = def->object->getName();
	edef.object.x = int(defpos.x);
	edef.object.y = int(defpos.z);

	eatk.attack.damageDealt = dmg;
	eatk.attack.attackedHealth = atk->object->getHealth();
	eatk.attack.attackedID = edef.object.id;
	eatk.attack.attackerID = eatk.object.id;
	eatk.attack.name = eatk.object.name;
	edef.attack = eatk.attack;

	this->pushEvent(eatk);
	this->pushEvent(edef);
}
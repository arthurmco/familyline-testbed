#include "Player.hpp"
#include "City.hpp"
#include <chrono>

using namespace familyline;
using namespace familyline::logic;

Player::Player(const char* name, int xp, GameActionManager* gam)
    : _name{name},  _xp(xp), _gam(gam)
{
    Log::GetLog()->Write("player",
			 "Created player %s, %d XP",
            name, xp);
}

void Player::AddCity(City* c)
{
    this->_city = c;
}

City* Player::GetCity()
{
    return this->_city;
}

inline uint64_t player_get_timestamp()
{
    auto timestamp = std::chrono::high_resolution_clock::now();
    auto epoch = timestamp.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
}

// Auxiliar functions to register game actions
void Player::RegisterCreation(GameObject* go)
{
    GameAction a;
    a.timestamp = player_get_timestamp();
    a.type = GAT_CREATION;
    a.creation.object_id = go->getID();

    auto pos = go->getPosition();
    
    a.creation.x = pos.x;
    a.creation.z = pos.z;
    _gam->Push(a);
}

void Player::RegisterMove(GameObject* go, glm::vec2 position)
{
    GameAction a;
    a.timestamp = player_get_timestamp();
    a.type = GAT_MOVE;
    a.move.object_id = go->getID();
    
    auto pos = go->getPosition();
    a.move.oldx = pos.x;
    a.move.oldz = pos.z;
    
    a.move.newx = position.x;
    a.move.newz = position.y;

    _gam->Push(a);
}

void Player::RegisterAttack(GameObject* attacker, GameObject* attackee)
{
    GameAction a;
    a.timestamp = player_get_timestamp();
    a.type = GAT_ATTACK;

    auto atkpos = attacker->getPosition();
    auto defpos = attackee->getPosition();
    
    a.attack.attacker_id = attacker->getID();
    a.attack.attackerx = atkpos.x;
    a.attack.attackerz = atkpos.z;
    a.attack.attackee_id = attackee->getID();
    a.attack.attackeex = defpos.x;
    a.attack.attackeez = defpos.z;
    
    _gam->Push(a);
}

void Player::RegisterDestroy(GameObject* go)
{
    GameAction a;
    a.timestamp = player_get_timestamp();
    a.type = GAT_DESTROY;
    a.destroy.object_id = go->getID();

    _gam->Push(a);
}

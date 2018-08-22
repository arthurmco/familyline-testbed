#include "Player.hpp"
#include "City.hpp"
#include <chrono>

using namespace Familyline;
using namespace Familyline::Logic;

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
    a.creation.x = go->position.x;
    a.creation.z = go->position.z;
    _gam->Push(a);
}

void Player::RegisterMove(GameObject* go, glm::vec2 position)
{
    GameAction a;
    a.timestamp = player_get_timestamp();
    a.type = GAT_MOVE;
    a.move.object_id = go->getID();
    a.move.oldx = go->position.x;
    a.move.oldz = go->position.z;
    a.move.newx = position.x;
    a.move.newz = position.y;

    _gam->Push(a);
}

void Player::RegisterAttack(GameObject* attacker, GameObject* attackee)
{
    GameAction a;
    a.timestamp = player_get_timestamp();
    a.type = GAT_ATTACK;
    a.attack.attacker_id = attacker->getID();
    a.attack.attackerx = attacker->position.x;
    a.attack.attackerz = attacker->position.z;
    a.attack.attackee_id = attackee->getID();
    a.attack.attackeex = attackee->position.x;
    a.attack.attackeez = attackee->position.z;
    
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

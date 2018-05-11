#include "Player.hpp"
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
    a.creation.object_id = go->GetObjectID();
    a.creation.x = go->GetX();
    a.creation.z = go->GetZ();

    _gam->Push(a);
}

void Player::RegisterMove(GameObject* go, glm::vec2 position)
{
    GameAction a;
    a.timestamp = player_get_timestamp();
    a.type = GAT_MOVE;
    a.move.object_id = go->GetObjectID();
    a.move.oldx = go->GetX();
    a.move.oldz = go->GetZ();
    a.move.newx = position.x;
    a.move.newz = position.y;

    _gam->Push(a);
}

void Player::RegisterAttack(GameObject* attacker, GameObject* attackee)
{
    GameAction a;
    a.timestamp = player_get_timestamp();
    a.type = GAT_ATTACK;
    a.attack.attacker_id = attacker->GetObjectID();
    a.attack.attackerx = attacker->GetX();
    a.attack.attackerz = attacker->GetZ();
    a.attack.attackee_id = attackee->GetObjectID();
    a.attack.attackeex = attackee->GetX();
    a.attack.attackeez = attackee->GetZ();
    
    _gam->Push(a);
}

void Player::RegisterDestroy(GameObject* go)
{
    GameAction a;
    a.timestamp = player_get_timestamp();
    a.type = GAT_DESTROY;
    a.destroy.object_id = go->GetObjectID();

    _gam->Push(a);
}

const char* Player::GetName() const { return this->_name.c_str(); }
int Player::GetXP() const { return this->_xp; }

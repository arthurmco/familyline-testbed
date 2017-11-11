#include "GameActionManager.hpp"
#include <cstdio>

using namespace Tribalia::Logic;

void GameActionManager::Push(GameAction a)
{
    _actions.push(a);
    printf(" -- timestamp %lu", a.timestamp);
    switch (a.type) {
    case GAT_CREATION:
	printf(", object creation, id: %d at pos %2.fx%2.f",
	       a.creation.object_id, a.creation.x, a.creation.z);
	break;
    case GAT_MOVE:
	printf(", object move, id: %d from %.2fx%.2f to %.2fx%.2f",
	       a.move.object_id, a.move.oldx, a.move.oldz,
	       a.move.newx, a.move.newz);
	break;
    case GAT_ATTACK:
	printf(", object attack, attacker id %d, on %.2fx%.2f attacked %d on "
	       "%.2fx%.2f",
	       a.attack.attacker_id, a.attack.attackerx, a.attack.attackerz,
	       a.attack.attackee_id, a.attack.attackeex, a.attack.attackeez);
	break;
    case GAT_DESTROY:
	printf(", object destroyed, id %d", a.destroy.object_id);
	break;
    }

    puts("");
}

bool GameActionManager::Pop(GameAction& a)
{
    if (_actions.empty())
	return false;

    a = _actions.front();
    _actions.pop();

    return true;
}

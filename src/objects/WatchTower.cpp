#include "WatchTower.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;

/*  1000 HP, 1.2 baseAtk, 1.5 baseArmor, 1.8 building material, 1.12 bulding
    strength, 5 units of garrison capacity */
WatchTower::WatchTower(int oid, float x, float y, float z)
    : Building(oid, TID, "WatchTower", x, y, z, 1000, 1.2, 1.5, 1.8, 1.12, 5) {
        DEF_MESH("WatchTower.obj");

    }


/* Called on object initialization */
bool WatchTower::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool WatchTower::DoAction(void) {return true; }

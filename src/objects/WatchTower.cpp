#include "WatchTower.hpp"

using namespace Familyline;
using namespace Familyline::Logic;

WatchTower::WatchTower()
    : Building(0, "watchtower", "Watch Tower", 1000, 1000,
	       AttackAttributes(1.2))
{}

/*  1000 HP, 1.2 baseAtk, 1.5 baseArmor, 1.8 building material, 1.12 bulding
    strength, 5 units of garrison capacity */
WatchTower::WatchTower(int oid, float x, float y, float z)
  : Building(oid, "watchtower", "Watch Tower", 1000, 1000,
	     AttackAttributes(1.2))
{
    //      DEF_MESH("WatchTower.obj");
    this->position = glm::vec3(x, y, z);
}


/* Called on object initialization */
bool WatchTower::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool WatchTower::DoAction(void) {return true; }

CLONE_MACRO_CPP(WatchTower)
//ADD_COPY_CTOR(WatchTower)

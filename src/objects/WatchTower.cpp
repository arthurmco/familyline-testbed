#include "WatchTower.hpp"

using namespace familyline;
using namespace familyline::logic;

//static AttackAttributes atk(1.2, 1.5, glm::radians(180.0), 25, 1.2);

WatchTower::WatchTower()
    : GameObject("watchtower", "Watch Tower", glm::vec2(10, 10), 1000, 1000)
{
    DEF_MESH("WatchTower.obj");
}

/* Called on object initialization */
bool WatchTower::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool WatchTower::DoAction(void) {return true; }

CLONE_MACRO_CPP(WatchTower)
//ADD_COPY_CTOR(WatchTower)

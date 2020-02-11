#include <common/objects/WatchTower.hpp>
#include <cmath> // for M_PI

using namespace familyline;
using namespace familyline::logic;

//static AttackAttributes atk(1.2, 1.5, glm::radians(180.0), 25, 1.2);

WatchTower::WatchTower()
    : GameObject("watchtower", "Watch Tower", glm::vec2(10, 10), 1000, 1000)
{
    DEF_MESH("WatchTower.obj");

    this->cAttack = std::optional<AttackComponent>({
            this,

            // atkRanged, atkMelee, atkSiege, atkTransporter
            2.0, 1.0, 1.0, 1.5,
            // defRanged, defMelee, defSiege, defTransporter
            2.0, 4.0, 1.0, 1.0,
            // rotation, atkDistance, armor, range
            0, 50, 2, M_PI
        });
}

/* Called on object initialization */
bool WatchTower::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool WatchTower::DoAction(void) {return true; }

CLONE_MACRO_CPP(WatchTower)
//ADD_COPY_CTOR(WatchTower)

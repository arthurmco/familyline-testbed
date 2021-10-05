#include <cmath>  // for M_PI
#include <common/objects/WatchTower.hpp>

using namespace familyline;
using namespace familyline::logic;

// static AttackAttributes atk(1.2, 1.5, glm::radians(180.0), 25, 1.2);

WatchTower::WatchTower() : GameObject("watchtower", "Watch Tower", glm::vec2(10, 10), 1000, 1000)
{
    DEF_MESH("WatchTower.obj");

    // TODO: change to projectile attack!
    // TODO: the attack attribute might be better inside the rule.
    this->cAttack = std::make_optional<AttackComponent>(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 2.0,
            .attackSpeed   = 12.0,
            .precision     = 90.0,
            .maxAngle      = M_PI},
        std::vector<AttackRule>(
            {AttackRule{.minDistance = 0.0, .maxDistance = 50.0, .ctype = AttackTypeMelee{}}}));
    this->cAttack->setParent(this);

    /*
    this->cAttack = std::optional<AttackComponent>(
        {this,

         // atkRanged, atkMelee, atkSiege, atkTransporter
         2.0, 1.0, 1.0, 1.5,
         // defRanged, defMelee, defSiege, defTransporter
         2.0, 4.0, 1.0, 1.0,
         // rotation, atkDistance, armor, range
         0, 50, 2, M_PI});
    */
    this->cColony = std::make_optional(ColonyComponent());
}

/* Called on object initialization */
bool WatchTower::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool WatchTower::DoAction(void) { return true; }

CLONE_MACRO_CPP(WatchTower)
// ADD_COPY_CTOR(WatchTower)

#include <common/objects/Tent.hpp>

using namespace familyline;
using namespace familyline::logic;

// static AttackAttributes atk(0, 1, glm::radians(0.0), 0, 0);

Tent::Tent() : GameObject("tent", "Tent", glm::vec2(15, 15), 250, 250)
{
    DEF_MESH("Tent.obj");

    this->cAttack = std::make_optional<AttackComponent>(
        AttackAttributes{
            .attackPoints  = 0.0,
            .defensePoints = 2.0,
            .attackSpeed   = 1.0,
            .precision     = 1.0,
            .maxAngle      = M_PI},
        std::vector<AttackRule>(
            {AttackRule{.minDistance = 0.0, .maxDistance = 10.0, .ctype = AttackTypeMelee{}}}));
    this->cAttack->setParent(this);

    this->cColony = std::make_optional(ColonyComponent());
};

/* Called on object initialization */
bool Tent::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool Tent::DoAction(void) { return true; }

CLONE_MACRO_CPP(Tent)

// ADD_COPY_CTOR(Tent)

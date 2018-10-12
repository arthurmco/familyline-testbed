#include "Tent.hpp"

using namespace Familyline;
using namespace Familyline::Logic;

static AttackAttributes atk(0, 1, glm::radians(0.0), 0, 0);

Tent::Tent() : Building(0, "tent", "Tent", 250, 250, atk)
{
    DEF_MESH("Tent.obj");
};

/*  250 HP, no baseAtk, 1.0 baseArmor, 1.0 building material, 0deg and 0 units of range,
    0.95 bulding strength, 2 units of garrison capacity */
Tent::Tent(int oid, float x, float y, float z)
    : Building(0, "tent", "Tent", 250, 250, atk)
{
    DEF_MESH("Tent.obj");
    this->position = glm::vec3(x, y, z);
}

/* Called on object initialization */
bool Tent::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool Tent::DoAction(void) {return true; }

CLONE_MACRO_CPP(Tent)

//ADD_COPY_CTOR(Tent)

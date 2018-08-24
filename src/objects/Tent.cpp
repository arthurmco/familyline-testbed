#include "Tent.hpp"

using namespace Familyline;
using namespace Familyline::Logic;

Tent::Tent() : Building(0, "tent", "Tent", 250, 250,
			AttackAttributes(0.0)){};

/*  250 HP, no baseAtk, 1.0 baseArmor, 1.0 building material, 0.95 bulding
    strength, 2 units of garrison capacity */
Tent::Tent(int oid, float x, float y, float z)
    : Building(0, "tent", "Tent", 250, 250,
	       AttackAttributes(0.0))
{
//        DEF_MESH("Tent.obj");
    this->position = glm::vec3(x, y, z);
}

/* Called on object initialization */
bool Tent::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool Tent::DoAction(void) {return true; }

//ADD_COPY_CTOR(Tent)

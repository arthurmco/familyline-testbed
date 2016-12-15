#include "Tent.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;

/*  250 HP, no baseAtk, 1.0 baseArmor, 1.0 building material, 0.95 bulding
    strength, 2 units of garrison capacity */
Tent::Tent(int oid, float x, float y, float z)
    : Building(oid, TID, "Tent", x, y, z, 250, 0, 1.0, 1.0, 0.95, 2) {
        DEF_MESH("models/Tent.obj");

    }


/* Called on object initialization */
bool Tent::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool Tent::DoAction(void) {return true; }

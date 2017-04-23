#include "TestObject.hpp"

using namespace Tribalia::Logic;

/*  250 HP, 3 baseAtk, 1.0 baseArmor, 1.0 building material, 0.95 bulding
    strength, 2 units of garrison capacity */
TestObject::TestObject(int oid, float x, float y, float z)
    : Building(oid, TID, "TestObject", x, y, z, 250, 3, 1.0, 1.0, 0.95, 2) {
    _radius = 2;
}


/* Called on object initialization */
bool TestObject::Initialize() { return true; }

/* Called on each engine loop, when an action is performed */
bool TestObject::DoAction(void) {return true; }
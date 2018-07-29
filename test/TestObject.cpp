#include "TestObject.hpp"

using namespace Familyline::Logic;


TestObject::TestObject() : Building(1, "test-object", "TestObject",
				    250, 250, AttackAttributes()) {}

/*  250 HP, 3 baseAtk, 1.0 baseArmor, 1.0 building material, 0.95 bulding
    strength, 2 units of garrison capacity */
TestObject::TestObject(object_id_t oid, float x, float y, float z)
    : Building(oid, "test-object", "TestObject", 250, 250, AttackAttributes()) {
    this->position = glm::vec3(x, y, z);
}

/* Called on each engine loop, when an action is performed */
void TestObject::iterate()
{
    GameObject::iterate();
}

//TestObject::TestObject(const TestObject& o)
//{
//    this->CopyObject((GameObject*)this, (GameObject&)o);
//}

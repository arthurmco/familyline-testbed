#include "test_obj.hpp"

using namespace Familyline::Logic;

void ObjectTest::SetUp()
{
    _om = new ObjectManager();
}

void ObjectTest::TearDown()
{
    delete _om;
}


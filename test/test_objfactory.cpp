#include "test_objfactory.hpp"

using namespace Tribalia::Logic;

void ObjectFactoryTest::SetUp()
{
    _of = Tribalia::Logic::ObjectFactory::GetInstance();
    _of->AddObject(new TestObject());
}

void ObjectFactoryTest::TearDown()
{
    delete _of;
}

TEST_F(ObjectFactoryTest, TestIfCanGetObject) {
    GameObject* go = _of->GetObject(TestObject::TID);

    ASSERT_NE(go, nullptr);
}

TEST_F(ObjectFactoryTest, TestIfAttributesArentLinked) {
    GameObject* go1 = _of->GetObject(TestObject::TID);
    GameObject* go2 = _of->GetObject(TestObject::TID);

    go1->AddProperty("malakoi", 200);

    ASSERT_FALSE(go2->HasProperty("malakoi")) << "The properties were cloned. You might need to fix the object copy constructor";

    go2->AddProperty("malakoi", 200);

    ASSERT_NE(go1->GetProperty<int>("malakoi"), go2->GetProperty<int>("malakoi"));
}

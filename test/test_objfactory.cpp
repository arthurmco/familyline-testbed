#include "test_objfactory.hpp"

using namespace Tribalia::Logic;

void ObjectFactoryTest::SetUp()
{
    _of = Tribalia::Logic::ObjectFactory::GetInstance();
    _of->AddObject(new TestObject());
}

void ObjectFactoryTest::TearDown()
{
//    delete _of;
}

TEST_F(ObjectFactoryTest, TestIfCanGetObject) {
#undef GetObject
    GameObject* go = _of->GetObject(TestObject::TID, 1, 1, 1);

    ASSERT_NE(go, nullptr);

    delete go;
}

TEST_F(ObjectFactoryTest, TestIfAttributesArentLinked) {
    GameObject* go1 = _of->GetObject(TestObject::TID, 1, 1, 1);
    GameObject* go2 = _of->GetObject(TestObject::TID, 10, 1, 10);
#define GetObject GetObjectA
    go1->AddProperty("malakoi", 200);

    ASSERT_FALSE(go2->HasProperty("malakoi")) << "The properties were cloned. You might need to fix the object copy constructor";

    go2->AddProperty("malakoi", 100);

    ASSERT_NE(go1->GetProperty<int>("malakoi"),
	      go2->GetProperty<int>("malakoi"));
}

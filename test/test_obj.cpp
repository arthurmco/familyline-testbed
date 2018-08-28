/*  Unit test fixture for game objects
 *
 *  Copyright (C) 2017 Arthur Mendes.
 */

#include <gtest/gtest.h>

#include "logic/ObjectManager.hpp"
#include "logic/GameObject.hpp"
#include "logic/ObjectEventEmitter.hpp"
#include "logic/ObjectEventListener.hpp"
#include "TestObject.hpp"

class ObjectTest : public ::testing::Test {
protected:
    Familyline::Logic::ObjectManager* _om;

public:
    void SetUp();
    void TearDown();
};

using namespace Familyline::Logic;

void ObjectTest::SetUp()
{
    _om = new ObjectManager();
}

void ObjectTest::TearDown()
{
    delete _om;
    ObjectEventEmitter::clearEvents();
    ObjectEventEmitter::clearListeners();
}

TEST_F(ObjectTest, TestObjectProperties) {
    GameObject o(0, "object", "Object");

    ASSERT_EQ(0, o.getID());
    ASSERT_STREQ("object", o.getType());
    ASSERT_STREQ("Object", o.getName());
}

TEST_F(ObjectTest, TestObjectManagerRemove) {
    auto o1 = _om->addObject(new GameObject(0, "object", "Object 1"));
    auto o2 = _om->addObject(new GameObject(0, "object", "Object 2"));

    {
	auto so1 = o1.lock();
	auto so2 = o2.lock();

	ASSERT_EQ(1, so1->getID());
	ASSERT_STREQ("object", so1->getType());
	ASSERT_STREQ("Object 1", so1->getName());

	ASSERT_EQ(2, so2->getID());
	ASSERT_STREQ("object", so2->getType());
	ASSERT_STREQ("Object 2", so2->getName());

	_om->removeObject(so1);
    }
    
    auto ro2 = _om->getObject(2);
    auto ro1 = _om->getObject(1);

    auto sro2 = ro2.lock();

    ASSERT_TRUE(o1.expired()) << "Uhh? " << ro2.use_count() << " objects";
    ASSERT_FALSE(o2.expired());
    ASSERT_EQ(2, sro2->getID());
    ASSERT_STREQ("object", sro2->getType());
    ASSERT_STREQ("Object 2", sro2->getName());

}

TEST_F(ObjectTest, TestObjectManagerOps) {
    auto o1 = _om->addObject(new GameObject(0, "object", "Object 1"));
    auto o2 = _om->addObject(new GameObject(0, "object", "Object 2"));

    auto so1 = o1.lock();
    auto so2 = o2.lock();

    ASSERT_EQ(1, so1->getID());
    ASSERT_STREQ("object", so1->getType());
    ASSERT_STREQ("Object 1", so1->getName());

    ASSERT_EQ(2, so2->getID());
    ASSERT_STREQ("object", so2->getType());
    ASSERT_STREQ("Object 2", so2->getName());

    auto ro2 = _om->getObject(2);
    auto ro1 = _om->getObject(1);

    auto sro2 = ro2.lock();
    auto sro1 = ro1.lock();

    ASSERT_EQ(1, sro1->getID());
    ASSERT_STREQ("object", sro1->getType());
    ASSERT_STREQ("Object 1", sro1->getName());

    ASSERT_EQ(2, sro2->getID());
    ASSERT_STREQ("object", sro2->getType());
    ASSERT_STREQ("Object 2", sro2->getName());

}

TEST_F(ObjectTest, TestCreateEvent) {
    ObjectEventListener oel;
    ObjectEventEmitter::addListener(&oel);
    ASSERT_FALSE(oel.hasEvent());

    auto o1 = _om->addObject(new GameObject(0, "object", "Object 1"));
    auto o2 = _om->addObject(new GameObject(0, "object", "Object 2"));
    ObjectEventEmitter::distributeMessages();

    auto so1 = o1.lock();
    auto so2 = o2.lock();

    ASSERT_TRUE(oel.hasEvent());
    ObjectEvent e;
    oel.popEvent(e);
    ASSERT_EQ(ObjectEventType::ObjectCreated, e.type);
    ASSERT_EQ(so1->getID(), e.oid);
    ASSERT_EQ(so1->getID(), e.to.lock()->getID());
    ASSERT_STREQ(so1->getName(), e.to.lock()->getName());

    oel.popEvent(e);
    ASSERT_EQ(ObjectEventType::ObjectCreated, e.type);
    ASSERT_EQ(so2->getID(), e.oid);
    ASSERT_EQ(so2->getID(), e.to.lock()->getID());
    ASSERT_STREQ(so2->getName(), e.to.lock()->getName());

    ASSERT_FALSE(oel.popEvent(e));
}

TEST_F(ObjectTest, TestDestroyEvent) {
    ObjectEventListener oel;
    ObjectEventEmitter::addListener(&oel);
    ASSERT_FALSE(oel.hasEvent());

    auto o1 = _om->addObject(new GameObject(0, "object", "Object 1"));
    auto o2 = _om->addObject(new GameObject(0, "object", "Object 2"));
    ObjectEventEmitter::distributeMessages();

    auto so1 = o1.lock();
    auto so2 = o2.lock();
    
    ObjectEvent e;

    oel.popEvent(e);
    oel.popEvent(e);
    ASSERT_FALSE(oel.hasEvent());

    _om->removeObject(so1);
    ObjectEventEmitter::distributeMessages();

    ASSERT_TRUE(oel.popEvent(e));
    ASSERT_EQ(ObjectEventType::ObjectDestroyed, e.type);
    ASSERT_EQ(1, e.oid);
    ASSERT_FALSE(oel.hasEvent());

    _om->removeObject(so2);
    ObjectEventEmitter::distributeMessages();

    ASSERT_TRUE(oel.popEvent(e));
    ASSERT_EQ(ObjectEventType::ObjectDestroyed, e.type);
    ASSERT_EQ(2, e.oid);
    ASSERT_FALSE(oel.hasEvent());
}

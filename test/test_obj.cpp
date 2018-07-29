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

    ASSERT_EQ(1, o1->getID());
    ASSERT_STREQ("object", o1->getType());
    ASSERT_STREQ("Object 1", o1->getName());

    ASSERT_EQ(2, o2->getID());
    ASSERT_STREQ("object", o2->getType());
    ASSERT_STREQ("Object 2", o2->getName());

    _om->removeObject(o1);
    auto ro2 = _om->getObject(2);
    auto ro1 = _om->getObject(1);

    ASSERT_EQ(nullptr, ro1);

    ASSERT_NE(nullptr, ro2);
    ASSERT_EQ(2, ro2->getID());
    ASSERT_STREQ("object", ro2->getType());
    ASSERT_STREQ("Object 2", ro2->getName());

}

TEST_F(ObjectTest, TestObjectManagerOps) {
    auto o1 = _om->addObject(new GameObject(0, "object", "Object 1"));
    auto o2 = _om->addObject(new GameObject(0, "object", "Object 2"));

    ASSERT_EQ(1, o1->getID());
    ASSERT_STREQ("object", o1->getType());
    ASSERT_STREQ("Object 1", o1->getName());

    ASSERT_EQ(2, o2->getID());
    ASSERT_STREQ("object", o2->getType());
    ASSERT_STREQ("Object 2", o2->getName());

    auto ro2 = _om->getObject(2);
    auto ro1 = _om->getObject(1);

    ASSERT_EQ(1, ro1->getID());
    ASSERT_STREQ("object", ro1->getType());
    ASSERT_STREQ("Object 1", ro1->getName());

    ASSERT_EQ(2, ro2->getID());
    ASSERT_STREQ("object", ro2->getType());
    ASSERT_STREQ("Object 2", ro2->getName());

}

TEST_F(ObjectTest, TestCreateEvent) {
    ObjectEventListener oel;
    ObjectEventEmitter::addListener(&oel);
    ASSERT_FALSE(oel.hasEvent());

    auto o1 = _om->addObject(new GameObject(0, "object", "Object 1"));
    auto o2 = _om->addObject(new GameObject(0, "object", "Object 2"));
    ObjectEventEmitter::distributeMessages();

    ASSERT_TRUE(oel.hasEvent());
    ObjectEvent e;
    oel.popEvent(e);
    ASSERT_EQ(ObjectEventType::ObjectCreated, e.type);
    ASSERT_EQ(o1->getID(), e.oid);
    ASSERT_EQ(o1->getID(), e.to->getID());
    ASSERT_STREQ(o1->getName(), e.to->getName());

    oel.popEvent(e);
    ASSERT_EQ(ObjectEventType::ObjectCreated, e.type);
    ASSERT_EQ(o2->getID(), e.oid);
    ASSERT_EQ(o2->getID(), e.to->getID());
    ASSERT_STREQ(o2->getName(), e.to->getName());

    ASSERT_FALSE(oel.popEvent(e));
}

TEST_F(ObjectTest, TestDestroyEvent) {
    ObjectEventListener oel;
    ObjectEventEmitter::addListener(&oel);
    ASSERT_FALSE(oel.hasEvent());

    auto o1 = _om->addObject(new GameObject(0, "object", "Object 1"));
    auto o2 = _om->addObject(new GameObject(0, "object", "Object 2"));
    ObjectEventEmitter::distributeMessages();
    
    ObjectEvent e;

    oel.popEvent(e);
    oel.popEvent(e);
    ASSERT_FALSE(oel.hasEvent());

    _om->removeObject(o1);
    ObjectEventEmitter::distributeMessages();

    ASSERT_TRUE(oel.popEvent(e));
    ASSERT_EQ(ObjectEventType::ObjectDestroyed, e.type);
    ASSERT_EQ(1, e.oid);
    ASSERT_FALSE(oel.hasEvent());

    _om->removeObject(o2);
    ObjectEventEmitter::distributeMessages();
    
    ASSERT_TRUE(oel.popEvent(e));
    ASSERT_EQ(ObjectEventType::ObjectDestroyed, e.type);
    ASSERT_EQ(2, e.oid);
    ASSERT_FALSE(oel.hasEvent());
}

/*
 * Test things about cities and object city attribution
 *
 * Copyright (C) 2018 Arthur Mendes
 */

#include <gtest/gtest.h>

class CityTest : public ::testing::Test {
protected:

public:
    void SetUp() {}
    void TearDown();
};

#include <memory>

#include "TestObject.hpp"
#include "TestPlayer.hpp"
#include "logic/CityManager.hpp"
#include "logic/City.hpp"
#include "logic/ObjectEventEmitter.hpp"
#include "logic/ObjectEvent.hpp"

using namespace familyline::logic;

void CityTest::TearDown() {

    ObjectEventEmitter::clearEvents();
    ObjectEventEmitter::clearListeners();
    ObjectManager::setDefault(nullptr);
}

TEST_F(CityTest, TestCityAllocation) {
    TestPlayer tp("Test Player");
    City c(&tp, glm::vec3(1, 1, 0));

    c.iterate(); // Just to see if it doesn't segfaults.

    ASSERT_EQ(glm::vec3(1, 1, 0), c.getColor());
    ASSERT_STREQ("Test Player", c.getPlayer()->getName());
}

TEST_F(CityTest, TestCityEventTransmission) {

    ObjectEventListener test_oel;
    ObjectEventEmitter::addListener(&test_oel);

    ObjectManager om;
    ObjectManager::setDefault(&om);
    om.addObject(std::move(new TestObject{}));

    GameActionManager gam;
    
    TestPlayer tp("Test Player", &gam);
    City c(&tp, glm::vec3(1, 1, 0));

    GameAction ga = {};
    ga.type = GAT_CREATION;
    ga.creation.object_id = 1;
    ga.creation.owner_id = 1;
    gam.Push(ga);

    gam.ProcessListeners();
    ObjectEventEmitter::distributeMessages();
    om.iterateAll();
    c.iterate();
    
    ObjectEvent e;
    ASSERT_EQ(true, test_oel.hasEvent());
    ASSERT_EQ(true, test_oel.popEvent(e));
    ASSERT_EQ(ObjectEventType::ObjectCreated, e.type);

    ObjectEventEmitter::distributeMessages();

    ASSERT_EQ(true, test_oel.popEvent(e));
    ASSERT_EQ(ObjectEventType::ObjectCityChanged, e.type);
    ASSERT_STREQ("test-object", e.from.lock()->getType());
    ASSERT_EQ(&c, e.city);

    ObjectEventEmitter::distributeMessages();
    ASSERT_EQ(false, test_oel.popEvent(e));

}

TEST_F(CityTest, TestCityEventTransmissionOnCityManager) {

    
    ObjectEventListener test_oel;
    ObjectEventEmitter::addListener(&test_oel);

    ObjectManager om;
    ObjectManager::setDefault(&om);
    om.addObject(std::move(new TestObject{}));

    GameActionManager gam;
    
    TestPlayer tp("Test Player", &gam);
    auto team = std::make_shared<Team>(1, "Test team");
	
    CityManager cm;
    City* c = cm.createCity(&tp, glm::vec3(1, 1, 0), team);

    GameAction ga = {};
    ga.type = GAT_CREATION;
    ga.creation.object_id = 1;
    ga.creation.owner_id = 1;
    gam.Push(ga);

    gam.ProcessListeners();
    ObjectEventEmitter::distributeMessages();
    om.iterateAll();
    cm.updateCities();
    
    ObjectEvent e;
    ASSERT_EQ(true, test_oel.hasEvent());
    ASSERT_EQ(true, test_oel.popEvent(e));
    ASSERT_EQ(ObjectEventType::ObjectCreated, e.type);

    ObjectEventEmitter::distributeMessages();

    ASSERT_EQ(true, test_oel.popEvent(e));
    ASSERT_EQ(ObjectEventType::ObjectCityChanged, e.type);
    ASSERT_STREQ("test-object", e.from.lock()->getType());
    ASSERT_EQ(c, e.city);

    ObjectEventEmitter::distributeMessages();
    ASSERT_EQ(false, test_oel.popEvent(e));
}

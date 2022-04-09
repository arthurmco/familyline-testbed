#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <common/logic/logic_service.hpp>
#include <common/logic/object_path_manager.hpp>
#include <common/logic/terrain.hpp>
#include <memory>

#include "utils.hpp"

using namespace familyline::logic;

class ObjectPathManagerTest : public ::testing::Test
{
protected:
    TerrainFile tf;
    std::unique_ptr<Terrain> t;

    void SetUp() override
    {
        tf = TerrainFile{200, 200};
        t  = std::make_unique<Terrain>(tf);

        LogicService::getActionQueue()->clearEvents();
        LogicService::initDebugDrawer(new DummyDebugDrawer{*t});
        LogicService::initPathManager(*t);
    }

    // void TearDown() override {}
};

TEST_F(ObjectPathManagerTest, SimplePath)
{
    ObjectManager om;
    auto atkComp                 = std::optional<AttackComponent>();
    struct object_init objParams = {"test-obj", "Test Object", glm::vec2(2, 2), 100,
                                    100,        false,         [&]() {},        atkComp};

    auto o = make_object(objParams);
    o->setPosition(glm::vec3(10, 0, 10));
    auto oid = om.add(std::move(o));

    auto& opm = LogicService::getPathManager();
    auto no   = *om.get(oid);

    ASSERT_EQ(opm->pathCount(), 0);
    opm->doPathing(no, glm::vec2(30, 30));
    ASSERT_EQ(opm->pathCount(), 1);

    ASSERT_EQ(no->getPosition(), glm::vec3(10, 0, 10));

    opm->update(om);  // Created -> Pathing
    ASSERT_EQ(no->getPosition(), glm::vec3(10, 0, 10));
    opm->update(om);  // Pathing -> Traversing
    ASSERT_EQ(no->getPosition(), glm::vec3(10, 0, 10));
    opm->update(om);
    ASSERT_EQ(no->getPosition(), glm::vec3(11, 0, 11));

    for (auto i = 0; i <= 30 - 11; i++) {
        opm->update(om);
    }

    EXPECT_EQ(no->getPosition(), glm::vec3(30, 0, 30));
    ASSERT_EQ(opm->pathCount(), 1);

    opm->update(om);
    EXPECT_EQ(opm->pathCount(), 0);
}

TEST_F(ObjectPathManagerTest, BlockedPathmanager)
{
    ObjectManager om;
    auto atkComp                 = std::optional<AttackComponent>();
    struct object_init objParams = {"test-obj", "Test Object", glm::vec2(2, 2), 100,
                                    100,        false,         [&]() {},        atkComp};

    auto o = make_object(objParams);
    o->setPosition(glm::vec3(10, 0, 10));
    auto oid = om.add(std::move(o));

    auto& opm = LogicService::getPathManager();
    opm->blockBitmapArea(0, 15, 200, 1);
    auto no = *om.get(oid);

    ASSERT_EQ(opm->pathCount(), 0);
    opm->doPathing(no, glm::vec2(10, 30));
    ASSERT_EQ(opm->pathCount(), 1);

    ASSERT_EQ(no->getPosition(), glm::vec3(10, 0, 10));

    opm->update(om);  // Created -> Pathing
    ASSERT_EQ(no->getPosition(), glm::vec3(10, 0, 10));
    opm->update(om);  // Pathing -> Impossible
    ASSERT_EQ(no->getPosition(), glm::vec3(10, 0, 10));
    opm->update(om);  // Impossible -> Deleted
    ASSERT_EQ(no->getPosition(), glm::vec3(10, 0, 10));
    ASSERT_EQ(opm->pathCount(), 0);
    opm->update(om);
    ASSERT_EQ(no->getPosition(), glm::vec3(10, 0, 10));
    ASSERT_EQ(opm->pathCount(), 0);
}

TEST_F(ObjectPathManagerTest, CollisionTestPathmanager)
{
    ObjectManager om;
    auto atkComp                  = std::optional<AttackComponent>();
    struct object_init obj1Params = {"test-obj", "Test1", glm::vec2(2, 2), 100,
                                     100,        false,   [&]() {},        atkComp};
    auto o1                       = make_object(obj1Params);
    o1->setPosition(glm::vec3(10, 0, 10));
    auto o1id = om.add(std::move(o1));

    struct object_init obj2Params = {"test-obj", "Test2", glm::vec2(2, 2), 100,
                                     100,        false,   [&]() {},        atkComp};

    auto o2 = make_object(obj2Params);
    o2->setPosition(glm::vec3(30, 0, 10));
    auto o2id = om.add(std::move(o2));

    auto& opm = LogicService::getPathManager();
    auto no1  = *om.get(o1id);
    auto no2  = *om.get(o2id);

    ASSERT_EQ(opm->pathCount(), 0);

    opm->doPathing(no1, glm::vec2(30, 30));
    opm->doPathing(no2, glm::vec2(10, 30));

    ASSERT_EQ(opm->pathCount(), 2);

    ASSERT_EQ(no1->getPosition(), glm::vec3(10, 0, 10));
    ASSERT_EQ(no2->getPosition(), glm::vec3(30, 0, 10));

    opm->update(om);  // Created -> Pathing
    ASSERT_EQ(no1->getPosition(), glm::vec3(10, 0, 10));
    ASSERT_EQ(no2->getPosition(), glm::vec3(30, 0, 10));

    opm->update(om);  // Pathing -> Traversing
    ASSERT_EQ(no1->getPosition(), glm::vec3(10, 0, 10));
    ASSERT_EQ(no2->getPosition(), glm::vec3(30, 0, 10));

    opm->update(om);
    ASSERT_EQ(no1->getPosition(), glm::vec3(11, 0, 11));
    ASSERT_EQ(no2->getPosition(), glm::vec3(29, 0, 11));

    for (auto i = 0; i <= 30 - 11; i++) {
        opm->update(om);
        ASSERT_NE(no1->getPosition(), no2->getPosition());
    }
    ASSERT_EQ(opm->pathCount(), 2);
    opm->update(om);
    opm->update(om);
    opm->update(om);
    opm->update(om);

    EXPECT_EQ(no2->getPosition(), glm::vec3(10, 0, 30));
    opm->update(om);
    ASSERT_EQ(opm->pathCount(), 1);

    opm->update(om);
    opm->update(om);
    opm->update(om);

    EXPECT_EQ(no1->getPosition(), glm::vec3(30, 0, 30));

    opm->update(om);
    ASSERT_EQ(opm->pathCount(), 0);
}

TEST_F(ObjectPathManagerTest, CollisionTestPathmanagerWithBigObject)
{
    ObjectManager om;
    auto atkComp                  = std::optional<AttackComponent>();
    struct object_init obj1Params = {"test-obj", "Test1", glm::vec2(6, 6), 100,
                                     100,        false,   [&]() {},        atkComp};
    auto o1                       = make_object(obj1Params);
    o1->setPosition(glm::vec3(10, 0, 10));
    auto o1id = om.add(std::move(o1));

    struct object_init obj2Params = {"test-obj", "Test2", glm::vec2(3, 3), 100,
                                     100,        false,   [&]() {},        atkComp};

    auto o2 = make_object(obj2Params);
    o2->setPosition(glm::vec3(30, 0, 10));
    auto o2id = om.add(std::move(o2));

    auto& opm = LogicService::getPathManager();
    auto no1  = *om.get(o1id);
    auto no2  = *om.get(o2id);

    ASSERT_EQ(opm->pathCount(), 0);

    opm->doPathing(no1, glm::vec2(30, 30));
    opm->doPathing(no2, glm::vec2(10, 30));

    ASSERT_EQ(opm->pathCount(), 2);

    ASSERT_EQ(no1->getPosition(), glm::vec3(10, 0, 10));
    ASSERT_EQ(no2->getPosition(), glm::vec3(30, 0, 10));

    opm->update(om);  // Created -> Pathing
    ASSERT_EQ(no1->getPosition(), glm::vec3(10, 0, 10));
    ASSERT_EQ(no2->getPosition(), glm::vec3(30, 0, 10));

    opm->update(om);  // Pathing -> Traversing
    ASSERT_EQ(no1->getPosition(), glm::vec3(10, 0, 10));
    ASSERT_EQ(no2->getPosition(), glm::vec3(30, 0, 10));

    opm->update(om);
    ASSERT_EQ(no1->getPosition(), glm::vec3(11, 0, 11));
    ASSERT_EQ(no2->getPosition(), glm::vec3(29, 0, 11));

    for (auto i = 0; i <= 30 - 9; i++) {
        opm->update(om);
        EXPECT_NE(no1->getPosition(), no2->getPosition());
        EXPECT_NE(no1->getPosition() + glm::vec3(1, 0, 0), no2->getPosition());
        EXPECT_NE(no1->getPosition() - glm::vec3(1, 0, 0), no2->getPosition());
        EXPECT_NE(no1->getPosition() + glm::vec3(2, 0, 0), no2->getPosition());
        EXPECT_NE(no1->getPosition() - glm::vec3(2, 0, 0), no2->getPosition());
        EXPECT_NE(no1->getPosition() + glm::vec3(2, 0, 2), no2->getPosition());
        EXPECT_NE(no1->getPosition() - glm::vec3(2, 0, 2), no2->getPosition());
        EXPECT_NE(no1->getPosition() + glm::vec3(3, 0, 3), no2->getPosition());
        EXPECT_NE(no1->getPosition() - glm::vec3(3, 0, 3), no2->getPosition());
    }
    ASSERT_EQ(opm->pathCount(), 2);
    opm->update(om);

    for (auto i = 0; i < 7; i++) opm->update(om);

    EXPECT_EQ(no2->getPosition(), glm::vec3(10, 0, 30));
    opm->update(om);
    ASSERT_EQ(opm->pathCount(), 1);

    opm->update(om);
    opm->update(om);

    EXPECT_EQ(no1->getPosition(), glm::vec3(30, 0, 30));

    opm->update(om);
    ASSERT_EQ(opm->pathCount(), 0);
}

TEST_F(ObjectPathManagerTest, FourWayCollisionTest)
{
    ObjectManager om;
    auto atkComp                  = std::optional<AttackComponent>();
    struct object_init obj1Params = {"test-obj", "Test1", glm::vec2(2, 2), 100,
                                     100,        false,   [&]() {},        atkComp};
    auto o1                       = make_object(obj1Params);
    o1->setPosition(glm::vec3(10, 0, 10));
    auto o1id = om.add(std::move(o1));

    struct object_init obj2Params = {"test-obj", "Test2", glm::vec2(2, 2), 100,
                                     100,        false,   [&]() {},        atkComp};

    auto o2 = make_object(obj2Params);
    o2->setPosition(glm::vec3(30, 0, 10));
    auto o2id = om.add(std::move(o2));

    struct object_init obj3Params = {"test-obj", "Test3", glm::vec2(2, 2), 100,
                                     100,        false,   [&]() {},        atkComp};
    auto o3                       = make_object(obj3Params);
    o3->setPosition(glm::vec3(10, 0, 20));
    auto o3id = om.add(std::move(o3));

    struct object_init obj4Params = {"test-obj", "Test4", glm::vec2(2, 2), 100,
                                     100,        false,   [&]() {},        atkComp};

    auto o4 = make_object(obj4Params);
    o4->setPosition(glm::vec3(20, 0, 10));
    auto o4id = om.add(std::move(o4));

    auto& opm = LogicService::getPathManager();
    auto no1  = *om.get(o1id);
    auto no2  = *om.get(o2id);
    auto no3  = *om.get(o3id);
    auto no4  = *om.get(o4id);

    ASSERT_EQ(opm->pathCount(), 0);

    opm->doPathing(no1, glm::vec2(30, 30));
    opm->doPathing(no2, glm::vec2(10, 30));
    opm->doPathing(no3, glm::vec2(30, 20));
    opm->doPathing(no4, glm::vec2(20, 30));

    ASSERT_EQ(opm->pathCount(), 4);

    ASSERT_EQ(no1->getPosition(), glm::vec3(10, 0, 10));
    ASSERT_EQ(no2->getPosition(), glm::vec3(30, 0, 10));

    opm->update(om);  // Created -> Pathing
    ASSERT_EQ(no1->getPosition(), glm::vec3(10, 0, 10));
    ASSERT_EQ(no2->getPosition(), glm::vec3(30, 0, 10));

    opm->update(om);  // Pathing -> Traversing
    ASSERT_EQ(no1->getPosition(), glm::vec3(10, 0, 10));
    ASSERT_EQ(no2->getPosition(), glm::vec3(30, 0, 10));

    opm->update(om);
    ASSERT_EQ(no1->getPosition(), glm::vec3(11, 0, 11));
    ASSERT_EQ(no2->getPosition(), glm::vec3(29, 0, 11));

    for (auto i = 0; i <= 30 - 11; i++) {
        opm->update(om);
        ASSERT_NE(no1->getPosition(), no2->getPosition());
        ASSERT_NE(no2->getPosition(), no3->getPosition());
        ASSERT_NE(no3->getPosition(), no4->getPosition());
    }
    ASSERT_EQ(opm->pathCount(), 4);

    for (auto i = 0; i < 13; i++) opm->update(om);

    EXPECT_EQ(no1->getPosition(), glm::vec3(30, 0, 30));
    EXPECT_EQ(no2->getPosition(), glm::vec3(10, 0, 30));
    EXPECT_EQ(no3->getPosition(), glm::vec3(30, 0, 20));
    EXPECT_EQ(no4->getPosition(), glm::vec3(20, 0, 30));

    opm->update(om);
    ASSERT_EQ(opm->pathCount(), 1);

    opm->update(om);
    opm->update(om);

    EXPECT_EQ(no1->getPosition(), glm::vec3(30, 0, 30));

    opm->update(om);
    ASSERT_EQ(opm->pathCount(), 0);
}

TEST(ObjectPathManagerTestExtra, EnormousPathfinding)
{
    auto tf = TerrainFile{550, 550};
    auto t  = std::make_unique<Terrain>(tf);

    LogicService::getActionQueue()->clearEvents();
    LogicService::initDebugDrawer(new DummyDebugDrawer{*t});
    LogicService::initPathManager(*t);

    ObjectManager om;
    auto atkComp                 = std::optional<AttackComponent>();
    struct object_init objParams = {"test-obj", "Test Object", glm::vec2(2, 2), 100,
                                    100,        false,         [&]() {},        atkComp};

    auto o = make_object(objParams);
    o->setPosition(glm::vec3(5, 0, 5));
    auto oid = om.add(std::move(o));

    auto& opm = LogicService::getPathManager();
    auto no   = *om.get(oid);

    ASSERT_EQ(opm->pathCount(), 0);
    opm->doPathing(no, glm::vec2(530, 530));
    ASSERT_EQ(opm->pathCount(), 1);

    ASSERT_EQ(no->getPosition(), glm::vec3(5, 0, 5));

    opm->update(om);  // Created -> Pathing
    ASSERT_EQ(no->getPosition(), glm::vec3(5, 0, 5));
    opm->update(om);  // Pathing -> Traversing
    ASSERT_EQ(no->getPosition(), glm::vec3(5, 0, 5));
    opm->update(om);
    ASSERT_EQ(no->getPosition(), glm::vec3(6, 0, 6));

    for (auto i = 0; i <= 530 - 6; i++) {
        opm->update(om);
    }

    EXPECT_EQ(no->getPosition(), glm::vec3(530, 0, 530));
    ASSERT_EQ(opm->pathCount(), 1);

    opm->update(om);
    ASSERT_EQ(opm->pathCount(), 0);
}

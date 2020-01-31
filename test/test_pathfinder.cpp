#include <gtest/gtest.h>
#include "utils.hpp"

#include "../src/logic/logic_service.hpp"
#include "../src/logic/PathFinder.hpp"

using namespace familyline::logic;

TEST(Pathfinder, CanWalkStraightLine) {
    ObjectManager om;

    auto atkComp = std::optional<AttackComponent>();
    struct object_init objParams = {
        "test-obj", "Test Object", glm::vec2(3, 3), 100, 100, false,
        [&]() {}, atkComp
    };

    auto component = make_object(objParams);
    component->setPosition(glm::vec3(10, 1, 10));

    PathFinder pf(&om);
    pf.InitPathmap(100, 100);
    pf.UpdatePathmap(100, 100);

    auto path = pf.CreatePath(*component.get(), glm::vec2(20, 10));

    EXPECT_EQ(11, path.size());
    ASSERT_EQ(glm::vec2(10, 10), path[0]);
    ASSERT_EQ(glm::vec2(11, 10), path[1]);
    ASSERT_EQ(glm::vec2(12, 10), path[2]);
    ASSERT_EQ(glm::vec2(13, 10), path[3]);
    ASSERT_EQ(glm::vec2(14, 10), path[4]);
    ASSERT_EQ(glm::vec2(15, 10), path[5]);
    ASSERT_EQ(glm::vec2(16, 10), path[6]);
    ASSERT_EQ(glm::vec2(17, 10), path[7]);
    ASSERT_EQ(glm::vec2(18, 10), path[8]);
    ASSERT_EQ(glm::vec2(19, 10), path[9]);
    ASSERT_EQ(glm::vec2(20, 10), path[10]);

}

TEST(Pathfinder, CanWalkDiagonalLine) {
    ObjectManager om;

    auto atkComp = std::optional<AttackComponent>();
    struct object_init objParams = {
        "test-obj", "Test Object", glm::vec2(3, 3), 100, 100, false,
        [&]() {}, atkComp
    };

    auto component = make_object(objParams);
    component->setPosition(glm::vec3(10, 1, 10));

    PathFinder pf(&om);
    pf.InitPathmap(100, 100);
    pf.UpdatePathmap(100, 100);

    auto path = pf.CreatePath(*component.get(), glm::vec2(22, 22));

    EXPECT_EQ(13, path.size());
    ASSERT_EQ(glm::vec2(10, 10), path[0]);
    ASSERT_EQ(glm::vec2(11, 11), path[1]);
    ASSERT_EQ(glm::vec2(12, 12), path[2]);
    ASSERT_EQ(glm::vec2(13, 13), path[3]);
    ASSERT_EQ(glm::vec2(14, 14), path[4]);
    ASSERT_EQ(glm::vec2(15, 15), path[5]);
    ASSERT_EQ(glm::vec2(16, 16), path[6]);
    ASSERT_EQ(glm::vec2(17, 17), path[7]);
    ASSERT_EQ(glm::vec2(18, 18), path[8]);
    ASSERT_EQ(glm::vec2(19, 19), path[9]);
    ASSERT_EQ(glm::vec2(20, 20), path[10]);
    ASSERT_EQ(glm::vec2(21, 21), path[11]);
    ASSERT_EQ(glm::vec2(22, 22), path[12]);
}

TEST(Pathfinder, CanWalkAroundObstacle) {
    ObjectManager om;

    auto atkComp = std::optional<AttackComponent>();
    struct object_init objParams = {
        "test-obj", "Test Object", glm::vec2(3, 3), 100, 100, false,
        [&]() {}, atkComp
    };

    auto component = make_object(objParams);
    auto obstacle = make_object(objParams);

    component->setPosition(glm::vec3(10, 1, 10));
    obstacle->setPosition(glm::vec3(16, 1, 16));

    PathFinder pf(&om);
    pf.InitPathmap(100, 100);
    pf.UpdatePathmap(100, 100);

    auto path = pf.CreatePath(*component.get(), glm::vec2(22, 22));

    EXPECT_GT(13, path.size());
    ASSERT_EQ(glm::vec2(10, 10), path[0]);
    ASSERT_EQ(glm::vec2(11, 11), path[1]);
    ASSERT_EQ(glm::vec2(12, 12), path[2]);
    ASSERT_NE(glm::vec2(13, 13), path[3]);
    ASSERT_NE(glm::vec2(14, 14), path[4]);
    ASSERT_NE(glm::vec2(15, 15), path[5]);
    ASSERT_NE(glm::vec2(16, 16), path[6]);
    ASSERT_NE(glm::vec2(17, 17), path[7]);
    ASSERT_NE(glm::vec2(18, 18), path[8]);
    ASSERT_NE(glm::vec2(19, 19), path[9]);

    ASSERT_EQ(glm::vec2(22, 22), path[path.size() - 1]);
    
}


/// TODO: need to implement this later, because it does not redo the
/// pathing for each iteration like it should.
//TEST(ObjectPathManager, CanWalkAroundMovingObstacles) {
//    
//}

//TEST(ObjectPathManager, CanPathfindTwoObjectsSimultaneously) {
//    
//}

//TEST(ObjectPathManager, CanStopMovingOnImpossiblePath) {
//    
//}

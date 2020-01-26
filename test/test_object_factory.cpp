#include <gtest/gtest.h>
#include "utils.hpp"

#include "../src/logic/logic_service.hpp"
#include "../src/logic/ObjectFactory.hpp"

using namespace familyline::logic;

TEST(ObjectFactoryOps, ObjectTestCreate) {

    auto component1 = make_object({
        "test-obj-one", "Test Object 1", glm::vec2(3, 3), 100, 100, false,
        []() {}, std::optional<AttackComponent>()
    });
    auto component2 = make_object({
        "test-obj-two", "Test Object 2", glm::vec2(3, 3), 300, 300, false,
        []() {}, std::optional<AttackComponent>()
    });

    ObjectFactory of;
    of.AddObject(component1.get());
    of.AddObject(component2.get());

    auto o1 = of.GetObject("test-obj-one", 3, 100, 3);
    auto o2 = of.GetObject("test-obj-two", 5, 100, 5);

    EXPECT_EQ(100, o1->getHealth());
    EXPECT_EQ(300, o2->getHealth());
}

TEST(ObjectFactoryOps, ObjectNotRepeat) {

    auto component1 = make_object({
        "test-obj-one", "Test Object 1", glm::vec2(3, 3), 100, 100, false,
        []() {}, std::optional<AttackComponent>()
    });
    auto component2 = make_object({
        "test-obj-two", "Test Object 2", glm::vec2(3, 3), 300, 300, false,
        []() {}, std::optional<AttackComponent>()
    });

    ObjectFactory of;
    of.AddObject(component1.get());
    of.AddObject(component2.get());

    auto o1 = of.GetObject("test-obj-one", 3, 100, 3);
    auto o2 = of.GetObject("test-obj-one", 5, 100, 5);

    ASSERT_EQ(3, o1->getPosition().x);
    ASSERT_EQ(5, o2->getPosition().x);
    ASSERT_EQ(3, o1->getPosition().z);
    ASSERT_EQ(5, o2->getPosition().z);
}

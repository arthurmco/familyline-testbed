#include <gtest/gtest.h>
#include "utils.hpp"

#include "../src/logic/logic_service.hpp"
#include "../src/logic/object_manager.hpp"

using namespace familyline::logic;

TEST(ObjectFactoryOps, ObjectAcquireID) {

    auto atkComp = std::optional<AttackComponent>();
    struct object_init objParams = {
        "test-obj", "Test Object", glm::vec2(3, 3), 100, 100, false,
        []() {}, atkComp
    };

    auto component = make_object(objParams);

    ObjectManager om;
    auto id = om.add(std::move(component));

    ASSERT_GT(id, 0);

    auto retrievedComp = om.get(id);
    ASSERT_TRUE(retrievedComp.has_value());
    ASSERT_EQ(objParams.name, retrievedComp.value()->getName());
    
}

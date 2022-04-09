#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <common/logic/logic_service.hpp>
#include <common/logic/pathfinder.hpp>
#include <common/logic/terrain.hpp>

#include "utils.hpp"

using namespace familyline::logic;

// Custom formatters
namespace glm
{

std::ostream& operator<<(std::ostream& os, const vec2& val)
{
    return os << "(" << val.x << ", " << val.y << ")";
}

}  // namespace glm

TEST(Pathfinder, SimplePathfinding)
{
    std::vector<bool> bitmap(900, false);

    Pathfinder p{bitmap, 30, 30};
    auto path = *p.calculate(glm::vec2(10, 10), glm::vec2(20, 20));

    ASSERT_EQ(path.size(), 11);
    EXPECT_EQ(path[0], glm::vec2(10, 10));
    EXPECT_EQ(path[1], glm::vec2(11, 11));
    EXPECT_EQ(path[9], glm::vec2(19, 19));
    EXPECT_EQ(path[10], glm::vec2(20, 20));
}

TEST(Pathfinder, BlockedPathfinding)
{
    std::vector<bool> bitmap(900, false);

    for (auto i = 30 * 15; i < 30 * 16; i++) {
        bitmap[i] = true;
    }

    Pathfinder p{bitmap, 30, 30};
    auto path = p.calculate(glm::vec2(10, 10), glm::vec2(10, 20));

    ASSERT_FALSE(path.has_value());
}

TEST(Pathfinder, ObstacledPathfinding)
{
    using ::testing::Contains;
    using ::testing::Not;

    std::vector<bool> bitmap(900, false);
#define set(x, y) bitmap[y * 30 + x] = true;

    set(14, 14) set(15, 14) set(16, 14) set(14, 15) set(15, 15) set(16, 15) set(14, 16) set(15, 16)
        set(16, 16)

#undef set

            Pathfinder p{bitmap, 30, 30};
    auto path = *p.calculate(glm::vec2(10, 10), glm::vec2(20, 20));

    ASSERT_EQ(path.size(), 14);
    EXPECT_EQ(path[0], glm::vec2(10, 10));
    EXPECT_EQ(path[1], glm::vec2(11, 11));

    EXPECT_THAT(path, Not(Contains(glm::vec2(14, 14))));
    EXPECT_THAT(path, Not(Contains(glm::vec2(14, 15))));
    EXPECT_THAT(path, Not(Contains(glm::vec2(15, 14))));
    EXPECT_THAT(path, Not(Contains(glm::vec2(15, 15))));
    EXPECT_THAT(path, Not(Contains(glm::vec2(16, 16))));

    EXPECT_EQ(path[13], glm::vec2(20, 20));
}

TEST(Pathfinder, GiganticPathfinding)
{
    std::vector<bool> bitmap(900 * 900, false);

    Pathfinder p{bitmap, 900, 900};
    auto path = *p.calculate(glm::vec2(10, 10), glm::vec2(899, 899));

    ASSERT_GT(path.size(), 10);
    ASSERT_LT(path.size(), 899);
    auto s = path.size();

    EXPECT_EQ(path[0], glm::vec2(10, 10));
    EXPECT_EQ(path[1], glm::vec2(11, 11));
    EXPECT_EQ(path[s - 2], glm::vec2(898, 898));
    EXPECT_EQ(path[s - 1], glm::vec2(899, 899));
}

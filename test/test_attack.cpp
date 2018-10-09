/* Attack-defense logic test
 *
 * Copyright (C) 2018 Arthur Mendes
 *
 */

#include <gtest/gtest.h>

#include "TestObject.hpp"

class AttackTest : public ::testing::Test {
protected:

public:
    void SetUp(){}
    void TearDown(){}
};


TEST_F(AttackTest, TestBasicAttackOnRange) {

    TestObject a1(1, 10, 0, 10);
    TestObject a2(2, 20, 0, 20);

    ASSERT_EQ(250, a1.getCurrentLifePoints());
    ASSERT_EQ(250, a2.getCurrentLifePoints());

    auto atk = a1.doAttack(&a2);
    ASSERT_NE(0.0, atk);
    ASSERT_DOUBLE_EQ(2.0, atk);

}

TEST_F(AttackTest, TestBasicAttackOffRange) {

    TestObject a1(1, 5, 0, 5);
    TestObject a2(2, 40, 0, 40);

    ASSERT_EQ(250, a1.getCurrentLifePoints());
    ASSERT_EQ(250, a2.getCurrentLifePoints());

    auto atk = a1.doAttack(&a2);
    ASSERT_DOUBLE_EQ(0.0, atk);
}


TEST_F(AttackTest, TestBasicAttackOnRangeButTurnedToOtherSide) {

    TestObject a1(1, 10, 0, 10);
    TestObject a2(2, 20, 0, 20);

    a1.rotation = M_PI;

    ASSERT_EQ(250, a1.getCurrentLifePoints());
    ASSERT_EQ(250, a2.getCurrentLifePoints());

    auto atk = a1.doAttack(&a2);
    ASSERT_DOUBLE_EQ(0.0, atk);
}

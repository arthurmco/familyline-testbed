/* Attack-defense logic test
 *
 * Copyright (C) 2018 Arthur Mendes
 *
 */

#include <memory>
#include <gtest/gtest.h>

#include "TestObject.hpp"
#include "logic/ObjectManager.hpp"
#include "logic/CombatManager.hpp"

class AttackTest : public ::testing::Test {
protected:

public:
    void SetUp(){}
    void TearDown(){}
};

using namespace Familyline::Logic;

TEST_F(AttackTest, TestBasicAttackOnRange) {

    TestObject a1(1, 10, 0, 10);
    TestObject a2(2, 20, 0, 20);

    ASSERT_EQ(250, a1.getCurrentLifePoints());
    ASSERT_EQ(250, a2.getCurrentLifePoints());

    auto atk = a1.doAttack(&a2);
    ASSERT_TRUE(atk.has_value());
    ASSERT_NE(0.0, atk.value());
    ASSERT_DOUBLE_EQ(2.0, atk.value());

}


TEST_F(AttackTest, TestBasicAttackAndDamage) {

    TestObject a1(1, 10, 0, 10);
    TestObject a2(2, 20, 0, 20);

    ASSERT_EQ(250, a1.getCurrentLifePoints());
    ASSERT_EQ(250, a2.getCurrentLifePoints());

    auto atk = a1.doAttack(&a2);
    ASSERT_TRUE(atk.has_value());
    ASSERT_EQ(248, a2.doDamage(atk.value()));

}

TEST_F(AttackTest, TestBasicAttackCombatManager) {

    ObjectManager om;
    ObjectManager::setDefault(&om);
    
    TestObject* a1 = new TestObject(0, 10, 0, 10);
    TestObject* a2 = new TestObject(0, 20, 0, 20);

    ASSERT_EQ(250, a1->getCurrentLifePoints());
    ASSERT_EQ(250, a2->getCurrentLifePoints());

    std::weak_ptr<AttackableObject> wa1 =
	std::dynamic_pointer_cast<AttackableObject>(om.addObject(a1).lock());
    std::weak_ptr<AttackableObject> wa2 =
	std::dynamic_pointer_cast<AttackableObject>(om.addObject(a2).lock());

    CombatManager* cm = new CombatManager;
    CombatManager::setDefault(cm);
    cm->AddAttack(wa1, wa2);

    cm->DoAttacks(0);

    ASSERT_EQ(250, wa1.lock()->getCurrentLifePoints());
    ASSERT_EQ(248, wa2.lock()->getCurrentLifePoints());
    ASSERT_EQ(ObjectState::Hurt, wa2.lock()->getState());
    
    ObjectManager::setDefault(nullptr);
    CombatManager::setDefault(nullptr);
}


TEST_F(AttackTest, TestBasicAttackCombatManagerIsDying) {

    ObjectManager om;
    ObjectManager::setDefault(&om);
    
    TestObject* a1 = new TestObject(0, 10, 0, 10);
    TestObject* a2 = new TestObject(0, 20, 0, 20);

    ASSERT_EQ(250, a1->getCurrentLifePoints());
    ASSERT_EQ(250, a2->getCurrentLifePoints());

    std::weak_ptr<AttackableObject> wa1 =
	std::dynamic_pointer_cast<AttackableObject>(om.addObject(a1).lock());
    std::weak_ptr<AttackableObject> wa2 =
	std::dynamic_pointer_cast<AttackableObject>(om.addObject(a2).lock());

    CombatManager* cm = new CombatManager;
    CombatManager::setDefault(cm);
    
    cm->AddAttack(wa1, wa2);
    ASSERT_EQ(ObjectState::Created, wa2.lock()->getState());

    // 1 hit = 2dmg
    for (auto i = 0; i < 125; i++)
	cm->DoAttacks(0);

    ASSERT_EQ(250, wa1.lock()->getCurrentLifePoints());
    ASSERT_EQ(0, wa2.lock()->getCurrentLifePoints());
    ASSERT_EQ(ObjectState::Dying, wa2.lock()->getState());
    
    ObjectManager::setDefault(nullptr);
    CombatManager::setDefault(nullptr);
}


TEST_F(AttackTest, TestBasicAttackCombatManagerIsDead) {

    ObjectManager om;
    ObjectManager::setDefault(&om);
    
    TestObject* a1 = new TestObject(0, 10, 0, 10);
    TestObject* a2 = new TestObject(0, 20, 0, 20);

    ASSERT_EQ(250, a1->getCurrentLifePoints());
    ASSERT_EQ(250, a2->getCurrentLifePoints());

    std::weak_ptr<AttackableObject> wa1 =
	std::dynamic_pointer_cast<AttackableObject>(om.addObject(a1).lock());
    std::weak_ptr<AttackableObject> wa2 =
	std::dynamic_pointer_cast<AttackableObject>(om.addObject(a2).lock());

    CombatManager* cm = new CombatManager;
    CombatManager::setDefault(cm);    
    cm->AddAttack(wa1, wa2);

    // 1 hit = 2dmg
    for (auto i = 0; i < 125; i++)
	cm->DoAttacks(0);

    ASSERT_EQ(ObjectState::Dying, wa2.lock()->getState());
    cm->DoAttacks(0);

    ASSERT_EQ(ObjectState::Dead, wa2.lock()->getState());
    
    
    ObjectManager::setDefault(nullptr);
    CombatManager::setDefault(nullptr);
}



TEST_F(AttackTest, TestBasicAttackOffRange) {

    TestObject a1(1, 5, 0, 5);
    TestObject a2(2, 40, 0, 40);

    ASSERT_EQ(250, a1.getCurrentLifePoints());
    ASSERT_EQ(250, a2.getCurrentLifePoints());

    auto atk = a1.doAttack(&a2);
    ASSERT_FALSE(atk.has_value());
}


TEST_F(AttackTest, TestBasicAttackOnRangeButTurnedToOtherSide) {

    TestObject a1(1, 10, 0, 10);
    TestObject a2(2, 20, 0, 20);

    a1.rotation = M_PI;

    ASSERT_EQ(250, a1.getCurrentLifePoints());
    ASSERT_EQ(250, a2.getCurrentLifePoints());

    auto atk = a1.doAttack(&a2);
    ASSERT_FALSE(atk.has_value());
}

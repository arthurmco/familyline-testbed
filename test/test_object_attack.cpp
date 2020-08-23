#include <gtest/gtest.h>

#include <common/logic/attack_manager.hpp>
#include <common/logic/lifecycle_manager.hpp>
#include <common/logic/logic_service.hpp>
#include <optional>

#include "utils.hpp"

using namespace familyline::logic;

TEST(ObjectAttack, TestMeleeAttack)
{
    auto atkc1 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto defc1 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});

    auto atker = make_object({"obj1", "Object 1", glm::vec2(5, 5), 200, 200, true, []() {}, atkc1});
    auto defer = make_object({"obj2", "Object 2", glm::vec2(5, 5), 200, 200, true, []() {}, defc1});

    atker->setPosition(glm::vec3(1, 0, 1));
    defer->setPosition(glm::vec3(1, 0, 1));
    
    ASSERT_TRUE(atker->getAttackComponent()->isInAttackRange(defer->getAttackComponent().value()));

    auto res = atker->getAttackComponent()->doDirectAttack(defer->getAttackComponent().value());

    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(1.0, res.value());
}

TEST(ObjectAttack, TestDeathNotification)
{
    auto& actionQueue = LogicService::getActionQueue();
    actionQueue->clearEvents();

    auto atkc1 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto defc1 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});

    auto atker_s =
        make_object({"obj1", "Object 1", glm::vec2(5, 5), 200, 200, true, []() {}, atkc1});

    auto defer_s =
        make_object({"obj2", "Object 2", glm::vec2(5, 5), 200, 200, true, []() {}, defc1});

    ObjectManager om;
    ObjectLifecycleManager olm{om};

    atker_s->setPosition(glm::vec3(1, 0, 1));
    defer_s->setPosition(glm::vec3(1, 0, 1));

    auto atkerid = om.add(std::move(atker_s));
    auto deferid = om.add(std::move(defer_s));

    actionQueue->processEvents();
    actionQueue->processEvents();
    actionQueue->processEvents();
    actionQueue->processEvents();

    AttackManager am;

    {
        auto atker = om.get(atkerid).value();
        auto defer = om.get(deferid).value();

        olm.doRegister(atker);
        olm.doRegister(defer);

        olm.notifyCreation(atkerid);
        olm.notifyCreation(deferid);

        ASSERT_TRUE(
            atker->getAttackComponent()->isInAttackRange(defer->getAttackComponent().value()));

        am.doRegister(atkerid, atker->getAttackComponent().value());
        am.doRegister(deferid, defer->getAttackComponent().value());
        olm.update();
        olm.update();

        am.startAttack(atkerid, deferid);
        olm.update();

        for (auto i = 0; i < 200; i++) {
            am.processAttacks(olm);
            olm.update();
            actionQueue->processEvents();
        }

        EXPECT_EQ(0.0, defer->getHealth());

        for (auto i = 0; i <= 80; i++) {
            am.processAttacks(olm);
            olm.update();
            actionQueue->processEvents();
        }

        olm.update();
        actionQueue->processEvents();
    }

    ASSERT_FALSE(om.get(deferid).has_value());
}

TEST(ObjectAttack, NotAttackIfMeleeOutOfRange)
{
    auto atkc1 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto defc1 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});

    auto atker = make_object({"obj1", "Object 1", glm::vec2(5, 5), 200, 200, true, []() {}, atkc1});

    auto defer = make_object({"obj2", "Object 2", glm::vec2(5, 5), 200, 200, true, []() {}, defc1});

    atker->setPosition(glm::vec3(0, 0, 0));
    defer->setPosition(glm::vec3(10, 0, 10));

    ASSERT_FALSE(atker->getAttackComponent()->isInAttackRange(defer->getAttackComponent().value()));

    auto res = atker->getAttackComponent()->doDirectAttack(defer->getAttackComponent().value());

    ASSERT_FALSE(res.has_value());
}

TEST(ObjectAttack, StopAttackingWhenMeleeOutOfRange)
{
    auto& actionQueue = LogicService::getActionQueue();
    actionQueue->clearEvents();

    auto atkc1 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto defc1 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});

    auto atker_s =
        make_object({"obj1", "Object 1", glm::vec2(5, 5), 200, 200, true, []() {}, atkc1});

    auto defer_s =
        make_object({"obj2", "Object 2", glm::vec2(5, 5), 200, 200, true, []() {}, defc1});

    ObjectManager om;
    ObjectLifecycleManager olm{om};

    auto atkerid = om.add(std::move(atker_s));
    auto deferid = om.add(std::move(defer_s));

    actionQueue->processEvents();
    actionQueue->processEvents();
    actionQueue->processEvents();
    actionQueue->processEvents();

    AttackManager am;

    {
        auto atker = om.get(atkerid).value();
        auto defer = om.get(deferid).value();

        atker->setPosition(glm::vec3(1, 1, 1));
        defer->setPosition(glm::vec3(0, 0, 0));

        olm.doRegister(atker);
        olm.doRegister(defer);

        olm.notifyCreation(atkerid);
        olm.notifyCreation(deferid);

        ASSERT_TRUE(
            atker->getAttackComponent()->isInAttackRange(defer->getAttackComponent().value()));

        am.doRegister(atkerid, atker->getAttackComponent().value());
        am.doRegister(deferid, defer->getAttackComponent().value());
        olm.update();
        olm.update();

        am.startAttack(atkerid, deferid);
        olm.update();

        for (auto i = 0; i < 100; i++) {
            am.processAttacks(olm);
            olm.update();
            actionQueue->processEvents();
        }

        EXPECT_NE(0.0, defer->getHealth());

        am.processAttacks(olm);
        olm.update();
        actionQueue->processEvents();

        auto hp = defer->getHealth();
        defer->setPosition(glm::vec3(10, 0, 10));

        am.processAttacks(olm);
        olm.update();
        actionQueue->processEvents();

        EXPECT_EQ(hp, defer->getHealth());
    }
}

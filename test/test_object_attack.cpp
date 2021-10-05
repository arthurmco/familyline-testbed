#include <gtest/gtest.h>

#include <cmath>
#include <common/logic/attack_manager.hpp>
#include <common/logic/lifecycle_manager.hpp>
#include <common/logic/logic_service.hpp>
#include <cstdint>
#include <glm/fwd.hpp>
#include <initializer_list>
#include <optional>
#include <variant>
#include <vector>

#include "common/logic/game_event.hpp"
#include "common/logic/game_object.hpp"
#include "common/logic/input_reproducer.hpp"
#include "common/logic/object_components.hpp"
#include "common/logic/object_manager.hpp"
#include "utils.hpp"

using namespace familyline::logic;

/****************/
TEST(AttackManager, BaseMeleeAttack)
{
    AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    auto atker =
        make_object({"atker", "Attacker", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});
    auto defer =
        make_object({"defder", "Defender", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});

    atker->setPosition(glm::vec3(4, 0, 4));

    defer->setPosition(glm::vec3(1, 0, 1));

    atk.setParent(atker.get());
    def.setParent(defer.get());

    ASSERT_TRUE(atk.isInRange(def));
    auto attack = atk.attack(def);
    ASSERT_TRUE(attack.has_value());
    ASSERT_EQ(AttackType::Melee, attack->atype);
    ASSERT_EQ(0.25, attack->value);
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, TooFarMeleeAttack)
{
    AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    auto atker =
        make_object({"atker", "Attacker", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});
    auto defer =
        make_object({"defder", "Defender", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});

    atker->setPosition(glm::vec3(6, 0, 6));
    defer->setPosition(glm::vec3(1, 0, 1));

    atk.setParent(atker.get());
    def.setParent(defer.get());

    ASSERT_FALSE(atk.isInRange(def));
    auto attack = atk.attack(def);
    ASSERT_FALSE(attack.has_value());
    ASSERT_EQ(AttackError::DefenderTooFar, attack.error());
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, BaseMeleeAttackWithEventPropagation)
{
    ObjectManager om;
    ObjectLifecycleManager olm{om};

    AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    auto atker =
        make_object({"atker", "Attacker", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});
    auto defer =
        make_object({"defder", "Defender", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});

    atker->setPosition(glm::vec3(4, 0, 4));
    defer->setPosition(glm::vec3(1, 0, 1));

    atk.setParent(atker.get());
    def.setParent(defer.get());

    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    AttackManager am;

    auto attack = atk.attack(def);
    ASSERT_TRUE(attack.has_value());

    LogicService::getActionQueue()->processEvents();

    EXPECT_EQ(AttackType::Melee, attack->atype);
    EXPECT_EQ(0.25, attack->value);

    std::queue<EntityEvent> events;
    auto test_recv = [&](const EntityEvent& e) {
        events.push(e);
        return true;
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::AttackMiss});

    EXPECT_EQ(0, events.size());
    am.update(om, olm);
    LogicService::getActionQueue()->processEvents();
    EXPECT_GE(events.size(), 0);
    EXPECT_LE(events.size(), 1);
    am.update(om, olm);
    LogicService::getActionQueue()->processEvents();
    ASSERT_GE(events.size(), 1);
    ASSERT_LE(events.size(), 2);

    auto e   = events.front();
    auto* ev = std::get_if<EventAttackDone>(&e.type);

    {
        auto atkptr = *om.get(atkid);
        auto defptr = *om.get(defid);

        ASSERT_TRUE(ev);
        EXPECT_EQ(atkid, ev->attackerID);
        EXPECT_EQ(defid, ev->defenderID);
        EXPECT_GT(ev->attackID, 0);
        EXPECT_EQ(0.25, ev->damageDealt);
        EXPECT_EQ(200.00, atkptr->getHealth());
        EXPECT_LE(defptr->getHealth(), 199.75);
        EXPECT_GE(defptr->getHealth(), 199.50);
    }

    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, TooFarMeleeAttackWithEventPropagation)
{
    ObjectManager om;
    ObjectLifecycleManager olm{om};

    AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    auto atker =
        make_object({"atker", "Attacker", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});
    auto defer =
        make_object({"defder", "Defender", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});

    atker->setPosition(glm::vec3(6, 0, 6));
    defer->setPosition(glm::vec3(1, 0, 1));

    atk.setParent(atker.get());
    def.setParent(defer.get());

    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    AttackManager am;

    ASSERT_FALSE(atk.isInRange(def));
    auto attack = atk.attack(def);
    ASSERT_FALSE(attack.has_value());
    ASSERT_EQ(AttackError::DefenderTooFar, attack.error());

    std::queue<EntityEvent> events;
    auto test_recv = [&](const EntityEvent& e) {
        events.push(e);
        return true;
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::AttackMiss});

    EXPECT_EQ(0, events.size());
    am.update(om, olm);
    LogicService::getActionQueue()->processEvents();
    EXPECT_EQ(0, events.size());
    am.update(om, olm);
    LogicService::getActionQueue()->processEvents();
    ASSERT_EQ(0, events.size());
    LogicService::getActionQueue()->processEvents();

    {
        auto atkptr = *om.get(atkid);
        auto defptr = *om.get(defid);

        EXPECT_EQ(200.00, atkptr->getHealth());
        EXPECT_EQ(200.00, defptr->getHealth());
    }

    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, IsBaseMeleeAttackNotFullyPreciseIfPrecisionIsLow)
{
    LogicService::getActionQueue()->clearEvents();

    ObjectManager om;
    ObjectLifecycleManager olm{om};

    AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    auto atker =
        make_object({"atker", "Attacker", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});
    auto defer =
        make_object({"defder", "Defender", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});

    atker->setPosition(glm::vec3(4, 0, 4));
    defer->setPosition(glm::vec3(1, 0, 1));

    atk.setParent(atker.get());
    def.setParent(defer.get());

    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    AttackManager am;

    auto attack = atk.attack(def);
    LogicService::getActionQueue()->processEvents();

    auto neededAttacksMin = int((200 / 0.25) / (100.0 / 100)) - 1;
    auto neededAttacksMax = int((200 / 0.25) / (90.0 / 100)) - 1;

    std::queue<EntityEvent> events_done;
    std::queue<EntityEvent> events_miss;
    auto test_recv = [&](const EntityEvent& e) {
        switch (e.type.index()) {
            case ActionQueueEvent::AttackDone: events_done.push(e); return true;
            case ActionQueueEvent::AttackMiss: events_miss.push(e); return true;
            default: return false;
        }
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::AttackMiss});

    for (auto i = 0; i < neededAttacksMin; i++) {
        LogicService::getActionQueue()->processEvents();
        am.update(om, olm);
    }
    LogicService::getActionQueue()->processEvents();

    ASSERT_GT(events_done.size(), 0);
    EXPECT_GE(events_done.size(), neededAttacksMin*0.9);
    EXPECT_LT(events_done.size(), neededAttacksMax);
    EXPECT_GT(events_miss.size(), 0);
    EXPECT_LE(events_miss.size(), neededAttacksMin*0.1);
    EXPECT_GE(events_miss.size(), neededAttacksMin*0.05);

    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, IsBaseMeleeAttackFullyPreciseIfPrecisionIs100)
{
    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();

    ObjectManager om;
    ObjectLifecycleManager olm{om};

    AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 100,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    auto atker =
        make_object({"atker", "Attacker", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});
    auto defer =
        make_object({"defder", "Defender", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});

    atker->setPosition(glm::vec3(4, 0, 4));
    defer->setPosition(glm::vec3(1, 0, 1));

    atk.setParent(atker.get());
    def.setParent(defer.get());

    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    AttackManager am;

    auto attack = atk.attack(def);

    auto neededAttacks = int((200 / 0.25) / (100.0 / 100)) - 1;

    std::queue<EntityEvent> events_done;
    std::queue<EntityEvent> events_miss;
    auto test_recv = [&](const EntityEvent& e) {
        switch (e.type.index()) {
            case ActionQueueEvent::AttackDone: events_done.push(e); return true;
            case ActionQueueEvent::AttackMiss: events_miss.push(e); return true;
            default: return false;
        }
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::AttackMiss});

    for (auto i = 0; i < neededAttacks; i++) {
        LogicService::getActionQueue()->processEvents();
        am.update(om, olm);
    }

    LogicService::getActionQueue()->processEvents();
    ASSERT_EQ(events_done.size(), neededAttacks);
    ASSERT_EQ(events_miss.size(), 0);

    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}

TEST(AttackManager, IsAttackSpeedRespected)
{
    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();

    ObjectManager om;
    ObjectLifecycleManager olm{om};

    AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048/16.0,
            .precision     = 100,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 8,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    auto atker =
        make_object({"atker", "Attacker", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});
    auto defer =
        make_object({"defder", "Defender", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});

    atker->setPosition(glm::vec3(4, 0, 4));
    defer->setPosition(glm::vec3(1, 0, 1));

    atk.setParent(atker.get());
    def.setParent(defer.get());

    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    AttackManager am;

    auto attack = atk.attack(def);

    auto hitAttacks = int((200 / 0.25) / (100.0 / 100)) - 1;
    auto neededIters = int((16*200 / 0.25) / (100.0 / 100)) - 16;

    std::queue<EntityEvent> events_done;
    std::queue<EntityEvent> events_miss;
    auto test_recv = [&](const EntityEvent& e) {
        switch (e.type.index()) {
            case ActionQueueEvent::AttackDone: events_done.push(e); return true;
            case ActionQueueEvent::AttackMiss: events_miss.push(e); return true;
            default: return false;
        }
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::AttackMiss});

    for (auto i = 0; i < neededIters; i++) {
        LogicService::getActionQueue()->processEvents();
        am.update(om, olm);
    }

    LogicService::getActionQueue()->processEvents();
    EXPECT_EQ(events_done.size(), hitAttacks);
    EXPECT_EQ(events_miss.size(), 0);


    {
        auto atkptr = *om.get(atkid);
        auto defptr = *om.get(defid);

        EXPECT_EQ(200.00, atkptr->getHealth());
        EXPECT_EQ(defptr->getHealth(), 0.25);
    }

    
    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}


TEST(AttackManager, IsDeadEventReceivedOnDeath)
{
    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();

    ObjectManager om;
    ObjectLifecycleManager olm{om};

    AttackComponent atk(
        AttackAttributes{
            .attackPoints  = 1.0,
            .defensePoints = 0.5,
            .attackSpeed   = 2048,
            .precision     = 100,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    AttackComponent def(
        AttackAttributes{
            .attackPoints  = 0.25,
            .defensePoints = 0.75,
            .attackSpeed   = 2048,
            .precision     = 90,
            .maxAngle      = M_PI},
        {AttackRule{.minDistance = 0.5, .maxDistance = 5, .ctype = AttackTypeMelee{}}});

    auto atker =
        make_object({"atker", "Attacker", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});
    auto defer =
        make_object({"defder", "Defender", glm::vec2(5, 5), 200, 200, true, []() {}, std::nullopt});

    atker->setPosition(glm::vec3(4, 0, 4));
    defer->setPosition(glm::vec3(1, 0, 1));

    atk.setParent(atker.get());
    def.setParent(defer.get());


    auto atkid = om.add(std::move(atker));
    auto defid = om.add(std::move(defer));

    {
        auto atkptr = *om.get(atkid);
        auto defptr = *om.get(defid);

        olm.doRegister(atkptr);
        olm.doRegister(defptr);
    }

    olm.notifyCreation(atkid);
    olm.notifyCreation(defid);
    olm.update();

    AttackManager am;

    auto attack = atk.attack(def);

    auto neededAttacks = int((200 / 0.25) / (100.0 / 100)) - 1;

    std::queue<EntityEvent> events_done;
    std::queue<EntityEvent> events_dead;
    auto test_recv = [&](const EntityEvent& e) {
        switch (e.type.index()) {
            case ActionQueueEvent::AttackDone: events_done.push(e); return true;
            case ActionQueueEvent::Dying: events_dead.push(e); return true;
            default: return false;
        }
    };

    LogicService::getActionQueue()->addReceiver(
        "test-receiver", test_recv, {ActionQueueEvent::AttackDone, ActionQueueEvent::Dying});

    for (auto i = 0; i < neededAttacks; i++) {
        LogicService::getActionQueue()->processEvents();
        am.update(om, olm);
        olm.update();
    }
    LogicService::getActionQueue()->processEvents();

    EXPECT_EQ(events_done.size(), neededAttacks);
    EXPECT_EQ(events_dead.size(), 0);

    am.update(om, olm);
    olm.update();
    LogicService::getActionQueue()->processEvents();

    EXPECT_EQ(events_done.size(), neededAttacks+1);
    EXPECT_EQ(events_dead.size(), 1);

    {
        auto atkptr = *om.get(atkid);
        auto defptr = *om.get(defid);

        EXPECT_EQ(200.00, atkptr->getHealth());
        EXPECT_LE(defptr->getHealth(), 0);
    }

    for (auto i = 0; i <= 80*10; i++) {
        LogicService::getActionQueue()->processEvents();
        am.update(om, olm);
        olm.update();

    }

    {
        auto atkptrv = om.get(atkid);
        auto defptrv = om.get(defid);

        EXPECT_TRUE(atkptrv);
        EXPECT_FALSE(defptrv);
    }


    LogicService::getActionQueue()->removeReceiver("test-receiver");
    LogicService::getActionQueue()->clearEvents();
}

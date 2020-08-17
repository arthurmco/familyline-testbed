#include <gtest/gtest.h>

#include <common/logic/ObjectPathManager.hpp>
#include <common/logic/PathFinder.hpp>
#include <common/logic/colony_manager.hpp>
#include <common/logic/lifecycle_manager.hpp>
#include <common/logic/logic_service.hpp>
#include <common/logic/player.hpp>
#include <common/logic/player_manager.hpp>
#include <common/logic/terrain.hpp>
#include <common/logic/terrain_file.hpp>

#include "utils.hpp"

using namespace familyline::logic;

void stepLogic(PlayerManager& pm, GameContext& gctx)
{
    pm.generateInput();
    pm.run(gctx);
    constexpr auto elapsed = 16 / 1000.0;

    gctx.tick++;
    gctx.elapsed_seconds += elapsed;
    ObjectPathManager::getInstance()->UpdatePaths(elapsed * 1000);
}

TEST(PlayerManager, TestIfPlayerCanBuild)
{
    LogicService::getObjectFactory()->clear();

    ObjectManager om;
    ObjectLifecycleManager olm{om};
    PathFinder pf{&om};

    bool object_rendered = false;

    PlayerManager pm;
    pm.olm = &olm;
    pm.pf  = &pf;

    auto atkc1 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto obj_s =
        make_object({"test", "Test Object", glm::vec2(0, 0), 200, 200, true, []() {}, atkc1});
    LogicService::getObjectFactory()->addObject(obj_s.get());

    TerrainFile tf{1,1};
    Terrain t{tf};

    auto d = std::make_unique<DummyPlayer>(pm, t, "Test", 1, [&]() -> std::vector<PlayerInputType> {
        return {EnqueueBuildAction{"test"}, CommitLastBuildAction{10.0, 12.0, 1.0, true}};
    });

    auto i = pm.add(std::move(d));
    ASSERT_NE(1, i);

    GameContext gctx       = {&om, 1, 0};
    pm.render_add_callback = [&](std::shared_ptr<GameObject> o) {
        auto pos = o->getPosition();
        ASSERT_FLOAT_EQ(10.0, pos.x);
        ASSERT_FLOAT_EQ(12.0, pos.z);
        ASSERT_FLOAT_EQ(1.0, pos.y);

        object_rendered = true;
    };
    pm.colony_add_callback = [](auto o, auto id) {};

    stepLogic(pm, gctx);

    ASSERT_TRUE(object_rendered);
}

TEST(PlayerManager, TestIfPlayerCanSelect)
{
    LogicService::getObjectFactory()->clear();

    ObjectManager om;
    ObjectLifecycleManager olm{om};
    PathFinder pf{&om};

    bool object_rendered = false;

    PlayerManager pm;
    pm.olm = &olm;
    pm.pf  = &pf;

    auto atkc1 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto obj_s1 =
        make_object({"test", "Test Object", glm::vec2(10, 10), 200, 200, true, []() {}, atkc1});

    auto atkc2 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto obj_s2 =
        make_object({"test2", "Test Object2", glm::vec2(20, 20), 200, 200, true, []() {}, atkc1});

    auto sid = om.add(obj_s1);
    om.add(obj_s2);

    TerrainFile tf{1,1};
    Terrain t{tf};
    
    auto d = std::make_unique<DummyPlayer>(pm, t, "Test", 1, [&]() -> std::vector<PlayerInputType> {
        return {
            ObjectSelectAction{sid},
        };
    });
    ASSERT_EQ(0, d->getSelections().size());

    auto i = pm.add(std::move(d));
    ASSERT_NE(1, i);

    GameContext gctx = {&om, 1, 0};
    stepLogic(pm, gctx);

    int iterated = 0;

    pm.iterate([&](Player* p) {
        iterated++;
        ASSERT_EQ(1, p->getSelections().size());

        auto sels = p->getSelections();
        auto sel0 = sels[0].lock();

        ASSERT_EQ(std::string{"Test Object"}, sel0->getName());
    });

    ASSERT_EQ(1, iterated);
}

TEST(PlayerManager, TestIfPlayerCanDeselect)
{
    LogicService::getObjectFactory()->clear();

    ObjectManager om;
    ObjectLifecycleManager olm{om};
    PathFinder pf{&om};

    bool object_rendered = false;

    PlayerManager pm;
    pm.olm = &olm;
    pm.pf  = &pf;

    auto atkc1 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto obj_s1 =
        make_object({"test", "Test Object", glm::vec2(10, 10), 200, 200, true, []() {}, atkc1});

    auto atkc2 = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto obj_s2 =
        make_object({"test2", "Test Object2", glm::vec2(20, 20), 200, 200, true, []() {}, atkc1});

    auto sid = om.add(obj_s1);
    om.add(obj_s2);

    GameContext gctx = {&om, 1, 0};
    
    TerrainFile tf{1,1};
    Terrain t{tf};
    
    auto d = std::make_unique<DummyPlayer>(pm, t, "Test", 1, [&]() -> std::vector<PlayerInputType> {
        switch (gctx.tick) {
            case 1:
                return {
                    ObjectSelectAction{sid},
                };
            default:
                return {
                    SelectionClearAction{},
                };
        }
    });

    auto i = pm.add(std::move(d));
    ASSERT_NE(1, i);

    int iterated = 0;

    auto iterFn = [&](Player* p) {
        iterated++;
        switch (gctx.tick) {
            case 2:
                ASSERT_EQ(1, p->getSelections().size())
                    << "Wrong selection on iteration " << iterated;
                break;
            default:
                ASSERT_EQ(0, p->getSelections().size())
                    << "Wrong deselection on iteration " << iterated;
        }
    };

    pm.iterate(iterFn);
    stepLogic(pm, gctx);
    pm.iterate(iterFn);
    stepLogic(pm, gctx);
    pm.iterate(iterFn);

    ASSERT_EQ(3, iterated);
}

TEST(PlayerManager, TestIfPlayerCannotMoveNotOwnedObject)
{
    LogicService::getObjectFactory()->clear();
    auto tf = std::make_unique<TerrainFile>(30, 30);
    Terrain t{*tf};
    ObjectPathManager::getInstance()->SetTerrain(&t);

    ColonyManager cm;
    ObjectManager om;
    ObjectLifecycleManager olm{om};
    PathFinder pf{&om};
    pf.InitPathmap(30, 30);

    bool object_rendered = false;

    PlayerManager pm;
    pm.olm = &olm;
    pm.pf  = &pf;

    auto atkc1  = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto obj_s1 = make_ownable_object(
        {"test", "Test Object", glm::vec2(10, 10), 200, 200, true, []() {}, atkc1});

    auto atkc2  = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto obj_s2 = make_ownable_object(
        {"test2", "Test Object2", glm::vec2(10, 10), 200, 200, true, []() {}, atkc1});

    obj_s2->setPosition(glm::vec3(20, 1, 20));

    auto id_s1 = om.add(obj_s1);
    auto sid   = om.add(obj_s2);

    GameContext gctx = {&om, 1, 0};

    
    auto d = std::make_unique<DummyPlayer>(pm, t, "Test", 1, [&]() -> std::vector<PlayerInputType> {
        switch (gctx.tick) {
            case 1:
                return {
                    ObjectSelectAction{sid},
                };
            case 2:
                return {
                    SelectedObjectMoveAction{15, 14},
                };
            default:
                return {
                    SelectionClearAction{},
                };
        }
    });

    auto& alliance = cm.createAlliance(std::string{"AAAA"});
    auto& colony   = cm.createColony(*d.get(), 0xff0000, std::optional{std::ref(alliance)});

    {
        (*om.get(id_s1))->getColonyComponent()->owner = std::ref(colony);
    }

    auto i = pm.add(std::move(d));
    ASSERT_NE(1, i);

    int iterated = 0;

    {
        auto obj  = om.get(sid);
        auto opos = (*obj)->getPosition();
        ASSERT_FLOAT_EQ(20.0, opos.x);
        ASSERT_FLOAT_EQ(20.0, opos.z);
    }

    stepLogic(pm, gctx);

    {
        auto obj  = om.get(sid);
        auto opos = (*obj)->getPosition();
        ASSERT_FLOAT_EQ(20.0, opos.x);
        ASSERT_FLOAT_EQ(20.0, opos.z);
    }

    stepLogic(pm, gctx);
    stepLogic(pm, gctx);
    stepLogic(pm, gctx);
    stepLogic(pm, gctx);
    stepLogic(pm, gctx);
    stepLogic(pm, gctx);
    stepLogic(pm, gctx);

    auto obj  = om.get(sid);
    auto opos = (*obj)->getPosition();

    ASSERT_FLOAT_EQ(20.0, opos.x);
    ASSERT_FLOAT_EQ(20.0, opos.z);

    // TODO: refactor the path manager PLEASE
    ObjectPathManager::getInstance()->SetTerrain(nullptr);
}

TEST(PlayerManager, TestIfPlayerCanMove)
{
    LogicService::getObjectFactory()->clear();
    auto tf = std::make_unique<TerrainFile>(30, 30);
    Terrain t{*tf};
    ObjectPathManager::getInstance()->SetTerrain(&t);

    ColonyManager cm;
    ObjectManager om;
    ObjectLifecycleManager olm{om};
    PathFinder pf{&om};
    pf.InitPathmap(30, 30);

    bool object_rendered = false;

    PlayerManager pm;
    pm.olm = &olm;
    pm.pf  = &pf;

    auto atkc1  = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto obj_s1 = make_ownable_object(
        {"test", "Test Object", glm::vec2(10, 10), 200, 200, true, []() {}, atkc1});

    auto atkc2  = std::optional<AttackComponent>(AttackComponent{
        nullptr, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 2.0f, 1.0f, 3.14f});
    auto obj_s2 = make_ownable_object(
        {"test2", "Test Object2", glm::vec2(10, 10), 200, 200, true, []() {}, atkc1});

    obj_s2->setPosition(glm::vec3(20, 1, 20));

    auto id_s1 = om.add(obj_s1);
    auto sid   = om.add(obj_s2);

    GameContext gctx = {&om, 1, 0};

    
    auto d = std::make_unique<DummyPlayer>(pm, t, "Test", 1, [&]() -> std::vector<PlayerInputType> {
        switch (gctx.tick) {
            case 1:
                return {
                    ObjectSelectAction{sid},
                };
            case 2:
                return {
                    SelectedObjectMoveAction{15, 14},
                };
            default:
                return {
                    SelectionClearAction{},
                };
        }
    });

    auto& alliance = cm.createAlliance(std::string{"AAAA"});
    auto& colony   = cm.createColony(*d.get(), 0xff0000, std::optional{std::ref(alliance)});

    {
        (*om.get(id_s1))->getColonyComponent()->owner = std::ref(colony);
        (*om.get(sid))->getColonyComponent()->owner   = std::ref(colony);
    }

    auto i = pm.add(std::move(d));
    ASSERT_NE(1, i);

    int iterated = 0;

    {
        auto obj  = om.get(sid);
        auto opos = (*obj)->getPosition();
        ASSERT_FLOAT_EQ(20.0, opos.x);
        ASSERT_FLOAT_EQ(20.0, opos.z);
    }

    stepLogic(pm, gctx);

    {
        auto obj  = om.get(sid);
        auto opos = (*obj)->getPosition();
        ASSERT_FLOAT_EQ(20.0, opos.x);
        ASSERT_FLOAT_EQ(20.0, opos.z);
    }

    stepLogic(pm, gctx);
    stepLogic(pm, gctx);
    stepLogic(pm, gctx);
    stepLogic(pm, gctx);
    stepLogic(pm, gctx);
    stepLogic(pm, gctx);
    stepLogic(pm, gctx);

    auto obj  = om.get(sid);
    auto opos = (*obj)->getPosition();

    ASSERT_FLOAT_EQ(15.0, opos.x);
    ASSERT_FLOAT_EQ(14.0, opos.z);

    // TODO: refactor the path manager PLEASE
    ObjectPathManager::getInstance()->SetTerrain(nullptr);
}

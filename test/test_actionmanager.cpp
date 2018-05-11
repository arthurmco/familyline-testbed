#include "test_actionmanager.hpp"

using namespace Familyline::Logic;

void TestActionManager::RunAction(const char* name) {
    Familyline::Logic::ActionData d;
    d.xPos = 50;
    d.yPos = 50;
    d.actionOrigin = tobj;

    auto it = _actions.find(name);
    if (it != _actions.end()) {
	ret[name] = it->second.handler(&it->second, d);
    }
}


void ActionManagerTest::SetUp()
{
    am = new TestActionManager();
}

void ActionManagerTest::TearDown()
{
    delete am;
}

TEST_F(ActionManagerTest, TestIfActionRuns) { 
    Action tac("test-action-1", "test-asset-1");
    bool did_ran = false;

    const char *acname = nullptr, *acasset = nullptr;
    size_t acref = 0;
    
    tac.handler = [&](Action* ac, ActionData acdata) -> bool
	{
	    acname = ac->name.c_str();
	    acasset = ac->assetname.c_str();
	    acref = ac->refcount;
	    
	    did_ran = true;
	    return true;
	};

    am->AddAction(tac);
    am->RunAction("test-action-1");
    ASSERT_TRUE(did_ran);
    ASSERT_NE(nullptr, acname);
    ASSERT_NE(nullptr, acasset);
    ASSERT_FALSE(strcmp(tac.name.c_str(), acname));
    ASSERT_FALSE(strcmp(tac.assetname.c_str(), acasset));
    ASSERT_EQ(1, acref);
}

TEST_F(ActionManagerTest, TestIfActionRemoves) {
    Action tac("test-action-1", "test-asset-1");
    bool did_ran = false;
    tac.handler = [&](Action* ac, ActionData acdata)
	{
	    (void)ac;
	    (void)acdata;
	    
	    did_ran = true;
	    return true;
	};

    am->AddAction(tac);
    
    am->RunAction("test-action-1");
    ASSERT_TRUE(did_ran);

    did_ran = false;
    am->RemoveAction("test-action-1");
    am->RunAction("test-action-1");

    ASSERT_FALSE(did_ran);
}

TEST_F(ActionManagerTest, TestIfActionVisible) {
    auto bare_handler = [](Action* ac, ActionData acdata)
	{
	    (void)ac;
	    (void)acdata;
	    
	    return true;
	};
    
    Action tacnv1("test-action-1", "test-asset-1");
    tacnv1.handler = bare_handler;

    Action tacnv2("test-action-2", "test-asset-2");
    tacnv2.handler = bare_handler;

    Action tac1("test-action-visible-1", "test-asset-1");
    tac1.handler = bare_handler;

    Action tac2("test-action-visible-2", "test-asset-2");
    tac2.handler = bare_handler;
    
    am->AddAction(tacnv1);
    am->AddAction(tac1);
    am->AddAction(tacnv2);
    am->AddAction(tac2);

    am->SetVisibleActions(std::vector<const char*>({"test-action-visible-1", "test-action-visible-2"}));

    std::vector<Action*> vact;
    am->GetVisibleActions(vact);
    ASSERT_EQ(2, vact.size());

    bool isv1 = false, isv2 = false;
    for (auto& it : vact) {
	if (!strcmp(it->name.c_str(), "test-action-visible-1")) {
	    isv1 = true;
	    continue;
	}

	if (!strcmp(it->name.c_str(), "test-action-visible-2")) {
	    isv2 = true;
	    continue;
	}
    }

    ASSERT_TRUE(isv1);
    ASSERT_TRUE(isv2);
}

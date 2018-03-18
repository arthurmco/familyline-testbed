#include "test_obj.hpp"

using namespace Tribalia::Logic;

void ObjectTest::SetUp()
{
    _om = new ObjectManager();
}

void ObjectTest::TearDown()
{
    delete _om;
}

TEST_F(ObjectTest, CheckObjectCreation){
    TestObject* t = new TestObject(-1, 10, 1, 10);
    ASSERT_EQ(10, t->GetX()) << "Wrong X position for object";
    ASSERT_EQ( 1, t->GetY()) << "Wrong Y position for object";
    ASSERT_EQ(10, t->GetZ()) << "Wrong Z position for object";

    _om->RegisterObject(t);
    EXPECT_NE(-1, t->GetObjectID()) << "ID not allocated correctly";

#ifdef _WIN32
#undef GetObject
#endif

    int id = t->GetObjectID();
    GameObject* tr = _om->GetObject(id);
    EXPECT_EQ(t, tr) << "Could not get object by ID";

    tr = _om->GetObject(10, 1, 10, 1);
    EXPECT_EQ(t, tr) << "Could not get object by position (low bias)";

    tr = _om->GetObject(11, 1, 11, 2);
    EXPECT_EQ(t, tr) << "Could not get object by position (high bias)";

    _om->UnregisterObject(tr);
    tr = _om->GetObject(id);
    EXPECT_EQ(nullptr, tr) << "Could not delete the object";

#ifdef _WIN32
#define GetObject GetObjectA
#endif

    delete t;
}

TEST_F(ObjectTest, CheckObjectProperties) {
    TestObject* o = new TestObject(-1, 1, 1, 1);
    const char* prop = "DandoBouraBandjido";
    
    ASSERT_EQ(false, o->HasProperty(prop));
    ASSERT_EQ(false, o->SetProperty<int>(prop, 0x101));

    o->AddProperty(prop, 0x101);
    ASSERT_EQ(true, o->HasProperty(prop));
    ASSERT_EQ(0x101, o->GetProperty<int>(prop));
        
    ASSERT_EQ(true, o->SetProperty(prop, 0x202));
    ASSERT_EQ(0x202, o->GetProperty<int>(prop));
    
}

TEST_F(ObjectTest, CheckObjectAttack){
    TestObject* atk = new TestObject(-1, 20, 1, 20);
    TestObject* def = new TestObject(-1, 20, 1, 10);

    _om->RegisterObject(atk);
    _om->RegisterObject(def);

    EXPECT_FALSE(atk->CheckAttackRange(def)) << "Attack is in range when it shouldn't";

    /* Check radius and attack range */
    EXPECT_EQ(2.0, atk->GetRadius()) << "Wrong radius";
    
    atk->SetZ(13); //TestObject has radius = 2
    EXPECT_TRUE(atk->CheckAttackRange(def)) << "Attack isn't in range when it should";

    for (int i = 0; i < 200; i++) { //due to randomness. TODO: better value
	atk->Hit(def, 0.1); /* The base attack is per 100 ms (0.1s) */
    }

    ASSERT_FLOAT_EQ(0.0, def->GetHP()) << "How is he still alive?";
    EXPECT_EQ(AST_DEAD, def->GetStatus()) << "0 life, but not dead";

    _om->UnregisterObject(def);
    _om->UnregisterObject(atk);

    delete def;
    delete atk;
}

#include "logic/CombatManager.hpp"

TEST_F(ObjectTest, CheckObjectAttackManagerSuspended){
    TestObject* atk = new TestObject(-1, 20, 1, 20);
    TestObject* def = new TestObject(-1, 20, 1, 10);

    _om->RegisterObject(atk);
    _om->RegisterObject(def);

    CombatManager::GetInstance()->AddAttack(atk, def);
	
    EXPECT_FALSE(atk->CheckAttackRange(def)) << "Attack is in range when it shouldn't";

    /* Check radius and attack range */
    EXPECT_EQ(2.0, atk->GetRadius()) << "Wrong radius";
    
    atk->SetZ(13); //TestObject has radius = 2
    EXPECT_TRUE(atk->CheckAttackRange(def)) << "Attack isn't in range when it should";
    bool death_called = false;

    CombatManager::GetInstance()->SetOnDeath([&](AttackableObject* o) {
	    (void)o;
	    
	    death_called = true;
	});

    for (int i = 0; i < 200; i++) { //due to randomness. TODO: better value
	CombatManager::GetInstance()->DoAttacks(0.1);

	if (i == 10) {
	    CombatManager::GetInstance()->SuspendAttack(atk);
	}
    }

    ASSERT_FALSE(death_called); // Test the callback

    ASSERT_LT(0.0, def->GetHP()) << "How is he dead?";
    EXPECT_NE(AST_DEAD, def->GetStatus()) << "He's dead. He shouldn't be";

    _om->UnregisterObject(def);
    _om->UnregisterObject(atk);

    delete def;
    delete atk;
}

TEST_F(ObjectTest, CheckObjectAttackEvenOnManager){
    TestObject* atk = new TestObject(-1, 20, 1, 20);
    TestObject* def = new TestObject(-1, 20, 1, 10);

    _om->RegisterObject(atk);
    _om->RegisterObject(def);

    CombatManager::GetInstance()->AddAttack(atk, def);
	
    EXPECT_FALSE(atk->CheckAttackRange(def)) << "Attack is in range when it shouldn't";

    /* Check radius and attack range */
    EXPECT_EQ(2.0, atk->GetRadius()) << "Wrong radius";
    
    atk->SetZ(13); //TestObject has radius = 2
    EXPECT_TRUE(atk->CheckAttackRange(def)) << "Attack isn't in range when it should";
    bool death_called = false;

    CombatManager::GetInstance()->SetOnDeath([&](AttackableObject* o) { death_called = true; });

    for (int i = 0; i < 200; i++) { //due to randomness. TODO: better value
	CombatManager::GetInstance()->DoAttacks(0.1);
    }

    ASSERT_TRUE(death_called); // Test the callback

    ASSERT_FLOAT_EQ(0.0, def->GetHP()) << "How is he still alive?";
    EXPECT_EQ(AST_DEAD, def->GetStatus()) << "0 life, but not dead";

    _om->UnregisterObject(def);
    _om->UnregisterObject(atk);

    delete def;
    delete atk;
}

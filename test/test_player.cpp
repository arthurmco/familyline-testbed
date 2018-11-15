#include "test_player.hpp"
#include "TestPlayer.hpp" //this is the class. Do not mistake.

using namespace familyline::logic;

void PlayerTest::SetUp()
{

}

void PlayerTest::TearDown()
{
}

/* Tests if the method is called correctly */
TEST_F(PlayerTest, TestPlayMethod) {
    TestPlayer p("One Test");
    p.Play(nullptr);
    ASSERT_EQ(1, p.getXP());
}

/* Test if player manager calls successfully the PlayAll() method */
TEST_F(PlayerTest, TestPlayerManagerPlayAll) {
    TestPlayer p("One Test");

    PlayerManager pm;
    pm.AddPlayer(&p);

    for (int i = 0; i < 3; i++)
	pm.PlayAll(nullptr);

    ASSERT_EQ(3, p.getXP());
}


/* Test if player manager queries some properties correctly*/
TEST_F(PlayerTest, TestPlayerManagerQueries) {

    TestPlayer p1("One Test");
    TestPlayer p2("Two Test");

    PlayerManager pm;
    auto id1 = pm.AddPlayer(&p1);
    auto id2 = pm.AddPlayer(&p2);

    ASSERT_STREQ(p1.getName(), pm.GetbyID(id1)->getName());
    ASSERT_STREQ(p2.getName(), pm.GetbyID(id2)->getName());

    ASSERT_STREQ(p1.getName(), pm.GetbyName("One Test")->getName());
    ASSERT_STREQ(p2.getName(), pm.GetbyName("Two Test")->getName());

}

/* Test if player manager plays all if multiple players are in */
TEST_F(PlayerTest, TestPlayerManagerPlayMulti) {

    TestPlayer p1("One Test");
    TestPlayer p2("Two Test");

    PlayerManager pm;
    pm.AddPlayer(&p1);

    for (int i = 0; i < 20; i++) {
	
	if (i == 10){
	    pm.AddPlayer(&p2);	    
	}

	pm.PlayAll(nullptr);
    }

    ASSERT_EQ(10, p2.getXP());
    ASSERT_EQ(20, p1.getXP());

}

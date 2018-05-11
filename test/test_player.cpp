#include "test_player.hpp"
#include "logic/Player.hpp"
#include "logic/PlayerManager.hpp"

using namespace Familyline::Logic;

/* This is a test player for this test suite
   In each play loop, it increases the XP by 1

   This spares a variable creation, we can use one already.
*/
class TestPlayer : public Player {
public:
    TestPlayer(const char* name)
	: Player(name, 0)
	{}

    virtual bool Play(GameContext*) {this->_xp++; return true; }
    virtual bool ProcessInput() { return true;}
    virtual bool HasUpdatedObject() { return true; }

};

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
    ASSERT_EQ(1, p.GetXP());
}

/* Test if player manager calls successfully the PlayAll() method */
TEST_F(PlayerTest, TestPlayerManagerPlayAll) {
    TestPlayer p("One Test");

    PlayerManager pm;
    pm.AddPlayer(&p);

    for (int i = 0; i < 3; i++)
	pm.PlayAll(nullptr);

    ASSERT_EQ(3, p.GetXP());
}


/* Test if player manager queries some properties correctly*/
TEST_F(PlayerTest, TestPlayerManagerQueries) {

    TestPlayer p1("One Test");
    TestPlayer p2("Two Test");

    PlayerManager pm;
    auto id1 = pm.AddPlayer(&p1);
    auto id2 = pm.AddPlayer(&p2);

    ASSERT_STREQ(p1.GetName(), pm.GetbyID(id1)->GetName());
    ASSERT_STREQ(p2.GetName(), pm.GetbyID(id2)->GetName());

    ASSERT_STREQ(p1.GetName(), pm.GetbyName("One Test")->GetName());
    ASSERT_STREQ(p2.GetName(), pm.GetbyName("Two Test")->GetName());

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

    ASSERT_EQ(10, p2.GetXP());
    ASSERT_EQ(20, p1.GetXP());

}

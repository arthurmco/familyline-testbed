#include "test_teams.hpp"

using namespace Familyline::Logic;

void TeamRelationTest::SetUp()
{
    tc = TeamCoordinator();
    c11 = new City("City 11", nullptr);
    c12 = new City("City 12", nullptr);
    c21 = new City("City 21", nullptr);
    c22 = new City("City 21", nullptr);
}

void TeamRelationTest::TearDown()
{
    if (c11) delete c11;
    if (c12) delete c12;
    if (c21) delete c21;
    if (c22) delete c22;
}

TEST_F(TeamRelationTest, TestIfSameTeamWorks){
    Team* t = tc.CreateTeam("Team");
    tc.AddTeam(t);
    tc.AddCity(t, c11);
    tc.AddCity(t, c12);
    tc.AddCity(t, c21);
    tc.AddCity(t, c22);

    EXPECT_EQ(t, c11->GetTeam());
    EXPECT_EQ(t, c12->GetTeam());
    EXPECT_EQ(t, c21->GetTeam());
    EXPECT_EQ(t, c22->GetTeam());
}

TEST_F(TeamRelationTest, TestIfDifferentTeamWorks){
    Team* t1 = tc.CreateTeam("Team1");
    Team* t2 = tc.CreateTeam("Team2");
    tc.AddTeam(t1);
    tc.AddTeam(t2);
    tc.AddCity(t1, c11);
    tc.AddCity(t1, c12);
    tc.AddCity(t2, c21);
    tc.AddCity(t2, c22);

    EXPECT_EQ(t1, c11->GetTeam());
    EXPECT_EQ(t1, c12->GetTeam());
    EXPECT_EQ(t2, c21->GetTeam());
    EXPECT_EQ(t2, c22->GetTeam());
    EXPECT_NE(t1, c21->GetTeam());
    EXPECT_NE(t1, c22->GetTeam());
    EXPECT_NE(t2, c11->GetTeam());
    EXPECT_NE(t2, c12->GetTeam());
    
}


TEST_F(TeamRelationTest, TestIfForkingTeamWorks) {
    Team* t1 = tc.CreateTeam("Team1");
    Team* t2 = tc.CreateTeam("Team2");
    tc.AddTeam(t1);
    tc.AddTeam(t2);
    tc.AddCity(t1, c11);
    tc.AddCity(t1, c12);
    tc.AddCity(t2, c21);
    tc.AddCity(t2, c22);

    /* Forking happens on city removal */
    tc.RemoveCity(c22);
    Team* t3 = c22->GetTeam();
    ASSERT_NE(nullptr, t3);
    
    t3->name = std::string("Team3");
    ASSERT_NE(t2->id, t3->id);

    EXPECT_EQ(t1, c11->GetTeam());
    EXPECT_EQ(t1, c12->GetTeam());
    EXPECT_EQ(t3, c22->GetTeam());
    EXPECT_NE(t3, c21->GetTeam());
    EXPECT_NE(t2, c22->GetTeam());
    EXPECT_NE(t1, c22->GetTeam());    
}

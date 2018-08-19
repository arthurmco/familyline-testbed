
#include "test_teams.hpp"
#include <vector>

using namespace Familyline::Logic;

void TeamRelationTest::SetUp()
{
    cm = new CityManager;
}

void TeamRelationTest::TearDown()
{
    delete cm;
}

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


TEST_F(TeamRelationTest, TestCityCreation){
    std::vector<std::shared_ptr<Team>> teams;

    auto team1 = std::make_shared<Team>(1, "The first ones");
    auto team2 = std::make_shared<Team>(2, "The last ones");
    teams.push_back(team1);
    teams.push_back(team2);

    team1->allies.push_back(team2);
    team2->allies.push_back(team1);

    TestPlayer* pl[4];
    for (int i = 0; i < 4; i++) {
	char cc[32];
	sprintf(cc, "Test Player %d", i);
	pl[i] = new TestPlayer(cc);
    }
    
    City* cities[4];
    cities[0] = cm->createCity(pl[0], glm::vec3(1.0, 0.3, 0.3), team1);
    cities[1] = cm->createCity(pl[1], glm::vec3(0.0, 1.0, 0.3), team2);
    cities[2] = cm->createCity(pl[2], glm::vec3(0.2, 0.6, 1.0), team2);
    cities[3] = cm->createCity(pl[3], glm::vec3(0.8, 0.4, 0.9), team1);

    ASSERT_EQ(pl[0], cities[0]->getPlayer());
    ASSERT_EQ(pl[1], cities[1]->getPlayer());
    ASSERT_EQ(pl[2], cities[2]->getPlayer());
    ASSERT_EQ(pl[3], cities[3]->getPlayer());
}

TEST_F(TeamRelationTest, TestAllies){
    std::vector<std::shared_ptr<Team>> teams;

    auto team1 = std::make_shared<Team>(1, "The first ones");
    auto team2 = std::make_shared<Team>(2, "The last ones");
    teams.push_back(team1);
    teams.push_back(team2);

    team1->allies.push_back(team2);
    team2->allies.push_back(team1);

    TestPlayer* pl[4];
    for (int i = 0; i < 4; i++) {
	char cc[32];
	sprintf(cc, "Test Player %d", i);
	pl[i] = new TestPlayer(cc);
    }

    City* cities[4];
    cities[0] = cm->createCity(pl[0], glm::vec3(1.0, 0.3, 0.3), team1);
    cities[1] = cm->createCity(pl[1], glm::vec3(0.0, 1.0, 0.3), team2);
    cities[2] = cm->createCity(pl[2], glm::vec3(0.2, 0.6, 1.0), team2);
    cities[3] = cm->createCity(pl[3], glm::vec3(0.8, 0.4, 0.9), team1);
    
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[0]->getDiplomacy(cities[3]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[1]->getDiplomacy(cities[2]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[2]->getDiplomacy(cities[1]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[3]->getDiplomacy(cities[0]));
	
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[2]->getDiplomacy(cities[3]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[0]->getDiplomacy(cities[2]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[3]->getDiplomacy(cities[1]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[1]->getDiplomacy(cities[0]));
}


TEST_F(TeamRelationTest, TestEnemies){
    std::vector<std::shared_ptr<Team>> teams;

    auto team1 = std::make_shared<Team>(1, "The first ones");
    auto team2 = std::make_shared<Team>(2, "The last ones");
    teams.push_back(team1);
    teams.push_back(team2);

    team1->enemies.push_back(team2);
    team2->enemies.push_back(team1);

    TestPlayer* pl[4];
    for (int i = 0; i < 4; i++) {
	char cc[32];
	sprintf(cc, "Test Player %d", i);
	pl[i] = new TestPlayer(cc);
    }

    City* cities[4];
    cities[0] = cm->createCity(pl[0], glm::vec3(1.0, 0.3, 0.3), team1);
    cities[1] = cm->createCity(pl[1], glm::vec3(0.0, 1.0, 0.3), team2);
    cities[2] = cm->createCity(pl[2], glm::vec3(0.2, 0.6, 1.0), team2);
    cities[3] = cm->createCity(pl[3], glm::vec3(0.8, 0.4, 0.9), team1);

    ASSERT_EQ(PlayerDiplomacy::Ally, cities[0]->getDiplomacy(cities[3]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[1]->getDiplomacy(cities[2]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[2]->getDiplomacy(cities[1]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[3]->getDiplomacy(cities[0]));
	
    ASSERT_EQ(PlayerDiplomacy::Enemy, cities[2]->getDiplomacy(cities[3]));
    ASSERT_EQ(PlayerDiplomacy::Enemy, cities[0]->getDiplomacy(cities[2]));
    ASSERT_EQ(PlayerDiplomacy::Enemy, cities[3]->getDiplomacy(cities[1]));
    ASSERT_EQ(PlayerDiplomacy::Enemy, cities[1]->getDiplomacy(cities[0]));
}


TEST_F(TeamRelationTest, TestNeutrals){
    std::vector<std::shared_ptr<Team>> teams;

    auto team1 = std::make_shared<Team>(1, "The first ones");
    auto team2 = std::make_shared<Team>(2, "The last ones");
    teams.push_back(team1);
    teams.push_back(team2);

    TestPlayer* pl[4];
    for (int i = 0; i < 4; i++) {
	char cc[32];
	sprintf(cc, "Test Player %d", i);
	pl[i] = new TestPlayer(cc);
    }

    City* cities[4];
    cities[0] = cm->createCity(pl[0], glm::vec3(1.0, 0.3, 0.3), team1);
    cities[1] = cm->createCity(pl[1], glm::vec3(0.0, 1.0, 0.3), team2);
    cities[2] = cm->createCity(pl[2], glm::vec3(0.2, 0.6, 1.0), team2);
    cities[3] = cm->createCity(pl[3], glm::vec3(0.8, 0.4, 0.9), team1);
    
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[0]->getDiplomacy(cities[3]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[1]->getDiplomacy(cities[2]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[2]->getDiplomacy(cities[1]));
    ASSERT_EQ(PlayerDiplomacy::Ally, cities[3]->getDiplomacy(cities[0]));
	
    ASSERT_EQ(PlayerDiplomacy::Neutral, cities[2]->getDiplomacy(cities[3]));
    ASSERT_EQ(PlayerDiplomacy::Neutral, cities[0]->getDiplomacy(cities[2]));
    ASSERT_EQ(PlayerDiplomacy::Neutral, cities[3]->getDiplomacy(cities[1]));
    ASSERT_EQ(PlayerDiplomacy::Neutral, cities[1]->getDiplomacy(cities[0]));
}


/***
    Team definitions inside a game

    Copyright (C) 2017 Arthur M

***/

#include <vector>
#include <algorithm>
#include <cstdint>

#include "City.hpp"

#ifndef TEAM_HPP
#define TEAM_HPP

namespace Tribalia {
namespace Logic {

struct TeamCities {
    Team team;
    std::list<City*> cities;

    /* Friends and foes list. Explicitly defined
       Every other one is neutral */
    std::list<Team*> friends;
    std::list<Team*> foes;

    TeamCities();
    TeamCities(Team team);
};

/* Diplomacy */
enum TeamDiplomacy {
    DIPLOMACY_NEUTRAL,
    DIPLOMACY_FRIEND,
    DIPLOMACY_FOE,
};
    
/***
    Manages teams' relations 
***/
class TeamCoordinator {
private:
    std::vector<TeamCities> teamlist;

    /* 'Forks' a city into a new team */
    Team* ForkCity(City* c);
public:
    TeamCoordinator();

    void AddTeam(Team* t);
    Team* CreateTeam(const char* name);
    
    bool AddCity(Team* t, City* c);

    /* Removes a city from a team.
       Note that, as a city *needs* to have a team, for correct friend-foe recognizement,
       we're going to create a new team just for it */
    void RemoveCity(City* c);

    TeamDiplomacy GetDiplomacyFor(Team* t1, Team* t2);

    /* Sets the diplomacy of t2 to 'dip' in t1.
       This doesn't set the other team diplomacy, this is a thing the other team 
       has to do */
    void SetDiplomacyFor(Team* t1, TeamDiplomacy dip, Team* t2);
};

}
}

#endif

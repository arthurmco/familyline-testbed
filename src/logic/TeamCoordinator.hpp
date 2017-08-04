/***
    Team coordinator definitions
    Manages team definitions, help team friend-foe evaluations and team
    creation and destruction

    Copyright (C) 2017 Arthur M

***/

#include <vector>

#include "Team.hpp"

#ifndef TEAMCOORDINATOR_HPP
#define TEAMCOORDINATOR_HPP

namespace Tribalia {
namespace Logic {

/***
    Manages teams' relations 
***/
class TeamCoordinator {
private:
    std::vector<TeamCities*> teamlist;

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

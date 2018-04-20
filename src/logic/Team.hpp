/***
    Team definitions inside a game

    Copyright (C) 2017 Arthur M

***/
#ifndef TEAM_HPP
#define TEAM_HPP

#include <vector>
#include <algorithm>
#include <cstdint>

#include "City.hpp"
#include <list>


namespace Tribalia {
namespace Logic {

struct TeamCities {
    Team* team;
    std::list<City*> cities;

    /* Friends and foes list. Explicitly defined
       Every other one is neutral */
    std::list<Team*> friends;
    std::list<Team*> foes;

    TeamCities();
    TeamCities(Team* team);
};

/* Diplomacy */
enum TeamDiplomacy {
    DIPLOMACY_NEUTRAL,
    DIPLOMACY_FRIEND,
    DIPLOMACY_FOE,
};
    
}
}

#endif

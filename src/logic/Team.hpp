/***
    Team definitions inside a game

    Copyright (C) 2017, 2018 Arthur M

***/
#ifndef TEAM_HPP
#define TEAM_HPP

#include <vector>

#include <list>
#include <memory> //weak_ptr, shared_ptr
#include <string>

namespace familyline::logic {

        /**
     * A team.
     *
     * Just a number, a name, possibly a flag?
     */
    struct Team {
	int number;
	std::string name;

	Team(int num, const char* name)
	    : number(num), name(name)
	    {}

	Team()
	    : number(-1), name()
	    {}

	// No copy constructor
	// Why would you copy a team?
	Team(const Team& o) = delete;

	//  Allies and enemies.
	//  Members of a team shouldn't attack ally teams
	//  Members of a team will attack enemy teams automatically if the units
	// are on Attack mode
	//  Other teams will be neutrals. Units should never attack neutrals automatically,
	// only the player should, manually, initiate the attack
	std::vector<std::weak_ptr<Team>> allies, enemies;
    };

    enum PlayerDiplomacy {
	Neutral,
	Ally,
	Enemy,
    };


}

#endif

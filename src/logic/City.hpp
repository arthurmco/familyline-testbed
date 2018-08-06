/***
	City definitions

	Copyright (C) 2016-2018 Arthur M

***/
#ifndef CITY_HPP
#define CITY_HPP


#include <vector>

#include "GameObject.hpp"
#include "Player.hpp"
#include "Log.hpp"
#include "Team.hpp"

#include <memory> //weak_ptr, shared_ptr
#include <glm/glm.hpp>

namespace Familyline::Logic {


    /**
     *! City class
     *
     * Defines a city.
     *
     * A city is a container for child objects of the game that belongs to the same user.
     *
     * A city should only have only one player linked to it (why?)
     *
     * Objects of a city should not attack each other, never.
     * Objects of a city should only attack objects of the other cities if they are
     * enemies or neutrals. Attacks to enemies might start automatically. Attacks to
     * neutrals should not
     *
     */
    class City : public GameObject {
    private:
	// TODO: Does the city have to own the game object too?
	// Nooooo. When the object is destroyed, the city should let it go.
	std::vector<GameObject*> citizens;

	// The player that controls the city.
	Player* player;

	// A color. This color is a RGB color.
	// We'll paint some parts of the units and buildings with this color
	// This color will also identify the user.
	glm::vec3 player_color;


    public:
	City(Player* player, glm::vec3 color)
	    : GameObject(0, "city", player->getName()), player(player), player_color(color)
	    {}

	std::shared_ptr<Team> team;

	PlayerDiplomacy getDiplomacy(City* c);

	const Player* getPlayer() const { return this->player; }
	const glm::vec3 getColor() const { return this->player_color; }

	// Check newly created objects?
	virtual void iterate();

    };


}



#endif /* end of include guard: CITY_HPP */

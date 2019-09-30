/***
    City definitions

    Copyright (C) 2016-2018 Arthur M

***/
#ifndef CITY_HPP
#define CITY_HPP


#include <vector>

#include "game_object.hpp"
#include "Log.hpp"
#include "Team.hpp"
#include "game_event.hpp"
#include "GameActionListener.hpp"

#include <memory> //weak_ptr, shared_ptr
#include <glm/glm.hpp>

namespace familyline::logic {

    class Player;

    class CityListener : public GameActionListener {
    private:
	std::queue<std::weak_ptr<GameObject>> obj_queue;
	int cityID;
    
    public:
	CityListener(Player* p, const char* name);

	virtual void OnListen(GameAction& ga);
	
	/*
	 * Get next created object, or nullptr if no object is next
	 */
	std::weak_ptr<GameObject> getNextObject();
	
	virtual ~CityListener() {}
    
    };

    
    /**
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

	/// Listens for objects, only send the set city event if the object is
	/// from its own city
	ObjectEventReceiver oel;
	
	// A color. This color is a RGB color.
	//
	// We'll paint some parts of the units and buildings with this color
	// This color will also identify the user.
	glm::vec3 player_color;

	/// Player listener, to get the newly created objects by the player
	CityListener* cil;

    public:
	City(Player* player, glm::vec3 color);

	std::shared_ptr<Team> team;

	PlayerDiplomacy getDiplomacy(City* c);

	/// The player that controls the city.
	const Player* getPlayer() const { return this->player; }

	/// The color that will identify the city
	///
	/// We'll paint the city objects with this color, so the
	/// players can easily identify them.
	const glm::vec3 getColor() const { return this->player_color; }


        /**
	 * Update all cities' objects
	 */
	virtual void iterate();

    };


}



#endif /* end of include guard: CITY_HPP */

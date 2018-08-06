/**
 * City manager.
 *
 * Receives object and player creation events and send them to
 * the appropriate cities
 *
 * Copyright (C) 2018 Arthur M
 */

#ifndef CITYMANAGER_HPP
#define CITYMANAGER_HPP

#include <list>
#include <memory>
#include <glm/glm.hpp>

#include "City.hpp"
#include "Player.hpp"

namespace Familyline::Logic {

    /**
     * Manages the current cities
     *
     * Gets the object creation events and player creation events, and sets the current city to them
     */
    class CityManager {
    private:
	std::list<std::unique_ptr<City>> cities;

    public:
	/**
	 * Creates a city and adds it in the city manager
	 */
	City* createCity(Player* p, glm::vec3 city_color, std::shared_ptr<Team> team);

	/**
	 * Check for player creation events received by the GameActionListener, crosscheck with
	 * the ones created by the ones received by the ObjectEventListener, and set the new 
	 * objects created by a player to its city.
	 */
	void updateCities();

	/**
	 * Gets the city with id 'city_id'
	 */
	City* getCity(int city_id);

    };
    
}  // Familyline::Logic


#endif /* CITYMANAGER_HPP */

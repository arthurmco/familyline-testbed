#include "CityManager.hpp"

using namespace Familyline::Logic;

/**
 * Creates a city and adds it in the city manager
 */
City* CityManager::createCity(Player* p, glm::vec3 city_color, std::shared_ptr<Team> team) {
    auto c = std::make_unique<City>(p, city_color);
    c->team = team;
    auto cc = c.get();
    cities.push_back(std::move(c));
    return cc;
}

/**
 * Check for player creation events received by the GameActionListener, crosscheck with
 * the ones created by the ones received by the ObjectEventListener, and set the new
 * objects created by a player to its city.
 */
void CityManager::updateCities() {
    // TODO: Add setCity event?

    // YES. An event named SET_CITY would be good.
}

/**
 * Gets the city with id 'city_id'
 */
City* CityManager::getCity(int city_id) {
    for (auto& c : cities) {
	if (c->getID() == city_id)
	    return c.get();
    }

    return nullptr;
}

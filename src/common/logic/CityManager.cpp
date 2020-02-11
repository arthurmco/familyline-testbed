#include <common/logic/CityManager.hpp>

using namespace familyline::logic;

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
 * Update all cities' objects
 */
void CityManager::updateCities() {
    for (auto& c : cities) {
	c->iterate();
    }
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

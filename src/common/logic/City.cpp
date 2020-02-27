#include <algorithm>
#include <fmt/format.h>

#include <common/logic/City.hpp>
#include <common/logic/object_manager.hpp>
#include <common/logic/game_event.hpp>
#include <common/logic/Player.hpp>

using namespace familyline;
using namespace familyline::logic;

// TODO: redo the city + player interaction

CityListener::CityListener(Player* p, const char* name)
    : GameActionListener(name)
{
    // p->_gam->AddListener(this);
}

void CityListener::OnListen(GameAction& ga) {

}

/*
 * Get next created object, or nullptr if no object is next
 */
std::weak_ptr<GameObject> CityListener::getNextObject() {
    return std::weak_ptr<GameObject>();
    
}

///////////////////////////////////////////////////

City::City(Player* player, glm::vec3 color)
{

    fmt::memory_buffer out;
    format_to(out, "city-listener-{:s}", player->getName());
    //cil = new CityListener{player, out.data()};
    
    //ObjectEventEmitter::addListener(&oel);
}

PlayerDiplomacy City::getDiplomacy(City* c) {

    // If the user is from your own team, he's an ally
    bool n_own_team = c->team->name == this->team->name;

    auto n_allies = std::count_if(team->allies.begin(), team->allies.end(),
                                  [&](auto& team) {
                                      auto steam = team.lock();
                                      return (c->team->name == steam->name);
                                  });
    auto n_enemies = std::count_if(team->enemies.begin(), team->enemies.end(),
                                   [&](auto& team) {
                                       auto steam = team.lock();
                                       return (c->team->name == steam->name);
                                   });

    if (n_allies > 0 || n_own_team)
        return PlayerDiplomacy::Ally;
    else if (n_enemies > 0)
        return PlayerDiplomacy::Enemy;
    else
        return PlayerDiplomacy::Neutral;

}

/**
 * Check for player creation events received by the GameActionListener, crosscheck with
 * the ones created by the ones received by the ObjectEventReceiver, and set the new 
 * objects created by a player to its city.
 */
void City::iterate() {

}

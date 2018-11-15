#include <algorithm>

#include "City.hpp"
#include "ObjectManager.hpp"
#include "ObjectEventEmitter.hpp"
#include "Player.hpp"

using namespace familyline;
using namespace familyline::logic;

CityListener::CityListener(Player* p, const char* name)
    : GameActionListener(name)
{
    p->_gam->AddListener(this);
}

void CityListener::OnListen(GameAction& ga) {
    if (ga.type == GAT_CREATION) {
	auto obj = ObjectManager::getDefault()->getObject(ga.creation.object_id);
	if (!obj.expired())
	    obj_queue.push(obj);
    }
}

/*
 * Get next created object, or nullptr if no object is next
 */
std::weak_ptr<GameObject> CityListener::getNextObject() {
    if (obj_queue.empty())
	return std::weak_ptr<GameObject>();

    auto obj = obj_queue.front();
    obj_queue.pop();
    return obj;
}

///////////////////////////////////////////////////

City::City(Player* player, glm::vec3 color)
    : GameObject(0, "city", player->getName()), player(player), player_color(color)
{

    char s[32];
    sprintf(s, "city-listener-%s", player->getName());
    cil = new CityListener{player, s};
    
    ObjectEventEmitter::addListener(&oel);
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
 * the ones created by the ones received by the ObjectEventListener, and set the new 
 * objects created by a player to its city.
 */
void City::iterate() {

    if (!oel.hasEvent())
	return; // No created object, no work.
    
    
    auto obj = cil->getNextObject();
    while (!obj.expired()) {
	auto sobj = obj.lock();

	ObjectEvent oevent;
	if (!oel.popEvent(oevent))
	    break;

	if (oevent.type == ObjectCreated && oevent.oid == sobj->getID()) {
	    // It's our object. Change its city to this
	    this->citizens.push_back(sobj.get());

	    ObjectEvent cevent(obj, this);
	    ObjectEventEmitter::pushMessage(nullptr, cevent);
	    break;
	}
	
	obj = cil->getNextObject();
    }
    
}

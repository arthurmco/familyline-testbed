#include <algorithm>

#include "City.hpp"

using namespace Familyline;
using namespace Familyline::Logic;

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

// Check newly created objects?
void City::iterate() {

}

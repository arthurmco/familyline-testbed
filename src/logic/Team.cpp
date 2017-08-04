#include "Team.hpp"

using namespace Tribalia::Logic;

TeamCities::TeamCities(Team team) {
    this->team = team;
}

TeamCities::TeamCities()
    : TeamCities(Team()) {this->team.tinfo = this;  }


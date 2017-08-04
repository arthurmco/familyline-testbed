#include "Team.hpp"

using namespace Tribalia::Logic;

TeamCities::TeamCities(Team* team) {
    this->team = team;
}

TeamCities::TeamCities()
    : TeamCities(new Team()) {this->team->tinfo = this; }


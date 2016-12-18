#include "Player.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;

Player::Player(const char* name, int elo, int xp)
    : _name{name}, _elo(elo), _xp(xp)
{
    Log::GetLog()->Write("Created player %s (%d XP, %d ELO ranking score)",
            name, elo, xp);
}

void Player::AddCity(City* c)
{
    _cities.push_back(c);
}

City* Player::GetCity()
{
    return _cities.at(0);
}

City* Player::GetCity(const char* name){

    for (auto it = _cities.begin(); it != _cities.end(); ++it){
        if (!strcmp((*it)->GetName(), name)){
            return (*it);
        }
    }

    return NULL;

}


const char* Player::GetName() { return this->_name.c_str(); }
int Player::GetXP() { return this->_xp; }

#include <common/logic/colony.hpp>

using namespace familyline::logic;

Colony::Colony(Player& p, unsigned color, Alliance& a)
    : player_(p), color_(color), alliance_(a)
{
    printf("colony for player %s created\n", p.getName().data());
    printf("added into alliance %s\n", a.name.c_str());
}



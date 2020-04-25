#include <common/logic/colony.hpp>
#include <common/logic/player.hpp>
#include <common/logger.hpp>

using namespace familyline::logic;

Colony::Colony(Player& p, unsigned color, Alliance& a)
    : player_(p), color_(color), alliance_(a)
{
    auto& log = LoggerService::getLogger();
    log->write("colony", LogType::Info, "colony for player %s created", p.getName().data());
    log->write("colony", LogType::Info, "added into alliance %s", a.name.c_str());
}


Alliance& Colony::getAlliance() const { return alliance_; }

std::string_view Colony::getName() const { return player_.getName(); }

bool Colony::isOfPlayer(Player& other)
{
    return player_.getCode() == other.getCode();
}

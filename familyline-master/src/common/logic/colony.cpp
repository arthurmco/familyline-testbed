#include <common/logger.hpp>
#include <common/logic/colony.hpp>
#include <common/logic/player.hpp>

using namespace familyline::logic;

Colony::Colony(Player& p, unsigned color, Alliance& a) : player_(p), color_(color), alliance_(a)
{
    auto& log = LoggerService::getLogger();
    log->write("colony", LogType::Info, "colony for player {} created", p.getName());
    log->write("colony", LogType::Info, "added into alliance {}", a.name);
}

Alliance& Colony::getAlliance() const { return alliance_; }

std::string_view Colony::getName() const { return player_.getName(); }

bool Colony::isOfPlayer(Player& other) { return player_.getCode() == other.getCode(); }

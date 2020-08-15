#include <common/logger.hpp>
#include <common/logic/colony_manager.hpp>
#include <common/logic/player.hpp>

using namespace familyline::logic;

ColonyManager::ColonyManager()
{
    colonies_.reserve(ColonyManager::MaxColonies);
    alliances_.reserve(ColonyManager::MaxColonies);
}

Colony& ColonyManager::createColony(
    Player& p, unsigned color,
    std::optional<std::reference_wrapper<Alliance>> alliance =
        std::optional<std::reference_wrapper<Alliance>>())
{
    if (colonies_.size() + 1 >= ColonyManager::MaxColonies) {
        throw std::runtime_error("maximum colony size reached!");
    }

    Alliance& a =
        alliance.has_value()
            ? alliance.value()
            : std::reference_wrapper<Alliance>(this->createAlliance(std::string{p.getName()}));

    colonies_.push_back(std::make_unique<Colony>(p, color, a));

    auto& c = colonies_.back();
    return *c.get();
}

Alliance& ColonyManager::createAlliance(std::string name)
{
    Alliance a;
    a.name = name;

    alliances_.push_back(a);

    auto& log = LoggerService::getLogger();
    log->write("colony-manager", LogType::Info, "alliance %s created", name.c_str());

    auto& ab = alliances_.back();
    return ab;
}

/**
 * Get diplomacy between colonies.
 */
DiplomacyStatus ColonyManager::getDiplomacy(Colony const& c, Colony const& other) const
{
    auto& alliance_c     = c.getAlliance();
    auto& alliance_other = other.getAlliance();

    if (std::find_if(alliance_c.enemies.begin(), alliance_c.enemies.end(), [&](auto alliance) {
            return alliance_other.getID() == alliance.get().getID();
        }) != alliance_c.enemies.end())
        return DiplomacyStatus::Enemy;

    if (std::find_if(alliance_c.allies.begin(), alliance_c.allies.end(), [&](auto alliance) {
            return alliance_other.getID() == alliance.get().getID();
        }) != alliance_c.allies.end())
        return DiplomacyStatus::Ally;

    return DiplomacyStatus::Neutral;
}

/**
 * Set diplomacy of the alliance `a` to `other`, how `a` views `other`
 *
 * This does not affect how `other` views `a`.
 */
void ColonyManager::setAllianceDiplomacy(Alliance& a, Alliance& other, DiplomacyStatus s)
{
    auto& log = LoggerService::getLogger();

    auto enemies_end = std::remove_if(a.enemies.begin(), a.enemies.end(), [&](auto alliance) {
        return other.getID() == alliance.get().getID();
    });

    auto allies_end = std::remove_if(a.allies.begin(), a.allies.end(), [&](auto alliance) {
        return other.getID() == alliance.get().getID();
    });

    a.enemies.erase(enemies_end, a.enemies.end());
    a.allies.erase(allies_end, a.allies.end());

    switch (s) {
        case DiplomacyStatus::Enemy:
            log->write(
                "colony-manager", LogType::Info, "%s is now enemy of %s", a.name.c_str(),
                other.name.c_str());
            a.enemies.push_back(other);
            break;

        case DiplomacyStatus::Ally:
            log->write(
                "colony-manager", LogType::Info, "%s is now ally of %s", a.name.c_str(),
                other.name.c_str());
            a.allies.push_back(other);
            break;

        default:
            log->write(
                "colony-manager", LogType::Info, "%s is now neutral of %s (have no diplomacy set)",
                a.name.c_str(), other.name.c_str());
            return;
    }
}

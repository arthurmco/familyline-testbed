#include <algorithm>
#include <common/logger.hpp>
#include <common/logic/terrain.hpp>

using namespace familyline::logic;

/**
 * Get height coords from a set of X and Y coords in the map
 */
unsigned Terrain::getHeightFromCoords(glm::vec2 coords) const
{
    auto [w, h] = tf_.getSize();
    auto idx    = int(coords.y) * w + int(coords.x);

    auto& data = tf_.getHeightData();
    return data[idx];
}

TerrainOverlay* Terrain::createOverlay(const char* name)
{
    std::string n{name};
    auto [w, h] = tf_.getSize();

    overlays_[n] = std::make_unique<TerrainOverlay>(w * h);

    auto& log = LoggerService::getLogger();
    log->write("terrain", LogType::Debug, "overlay '{}' created", name);

    return overlays_[n].get();
}

TerrainOverlay::TerrainOverlay(size_t size) : data_(std::vector<uint16_t>(size, 0)) {}

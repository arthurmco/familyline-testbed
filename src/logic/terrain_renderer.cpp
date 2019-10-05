#include "Terrain.hpp"
#include "terrain_renderer.hpp"

#include <glm/glm.hpp>

using namespace familyline::logic;

TerrainRenderer::TerrainRenderer(Terrain& t)
	: _terrain(t)
{
	//_terrain._renderer = this;
}


/**
 * Update the terrain: if needed, send the data from the current quadrant and
 * its neighbors to the video card.
 * But does not render yet
 */
void TerrainRenderer::update(const Camera& c)
{

}


/**
 * Render the current area of the terrain
 */
void TerrainRenderer::render(const Camera& c)
{

}

/**
 * Convert a coordinate from game space to model space
 */
glm::vec3 TerrainRenderer::convertToModelSpace(glm::vec3 terrainPos) const
{
	return terrainPos;
}


TerrainRenderer::~TerrainRenderer()
{
    //	_terrain._renderer = nullptr;
}

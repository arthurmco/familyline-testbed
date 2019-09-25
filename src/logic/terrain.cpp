#include "terrain.hpp"
#include "terrain_renderer.hpp"

using namespace familyline::logic;

Terrain::Terrain(int xsize, int zsize, const std::vector<TerrainSlot>& slots)
	: _xsize(xsize), _zsize(zsize), _slots(slots)
{}


glm::vec2 Terrain::getSize() const
{
	return glm::vec2(_xsize, _zsize);
}

/**
 * Get the height, in game space, from a certain point
 * 
 * Uses some interpolation math to get the correct height even between
 * two "terrain points"
 */
float Terrain::getHeightFromPoint(float x, float z) const
{
	auto& slotx1 = _slots[int(z) * _xsize + int(x)];
	auto& slotx2 = _slots[int(z) * _xsize + int(x) + 1];
	auto& sloty1 = _slots[int(z) * _xsize + int(x)];
	auto& sloty2 = _slots[(int(z) + 1) * _xsize + int(x)];

	float fx = x - int(x);
	float fz = z - int(z);

	auto r = (slotx2.y - slotx1.y) * fx + (sloty2.y - sloty1.y) * fz; 
	return r;
}

/**
 * Get the inclination, in game space, between two points
 *
 * Return the inclination angle in radians
 */
float Terrain::getInclinationFromPoint(glm::vec2 s, glm::vec2 e) const
{
	return 0;
}


const TerrainRenderer& Terrain::getRenderer() const
{
	return *_renderer;
}

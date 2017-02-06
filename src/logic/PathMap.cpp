#include "PathFinder.hpp"

using namespace Tribalia::Logic;

PathMap::PathMap(int w, int h)
{
    map = new PathMapElement[w*h];
    width = w;
    height = h;
}

PathMap::~PathMap()
{
    delete map;
}

PathItem::PathItem(glm::vec2 p, PathFinderSlot* s)
{
    point = p;
    slot = s;
}

void PathItem::calculateAStar(glm::vec2 from, glm::vec2 to)
{
    g = glm::distance(from, point);
    h = glm::distance(point, to);
    
    f = g + h;
}

    /* Calculate multiplication numbers */
void PathItem::calculateMult(bool isWaterUnit)
{
    float mult = 1.0f;
    
    
    if (slot->isObstructed) {
	mult = 9E+10;
    } else {
	/* TODO: Think of a better calculation */
	if (isWaterUnit)
	    mult = slot->elevation_points / slot->terrain_water_points;
	else
	    mult = slot->elevation_points * (slot->terrain_land_points);
    }
    
    f *= mult;
}

bool PathItem::operator==(const PathItem& p) const
{
    return (point == p.point);
}

bool PathItem::operator==(const glm::vec2& v) const
{
    return (point == v);
}

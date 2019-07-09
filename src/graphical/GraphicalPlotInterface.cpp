#include "GraphicalPlotInterface.hpp"
#include "TerrainRenderer.hpp"
#include <functional>

using namespace familyline::graphics;
using namespace familyline::logic;

GraphicalPlotInterface::GraphicalPlotInterface(Renderer* renderer)
    : _renderer(renderer)
{}

/* Adds a vector of points to form a path.
   Return a path handle */
PathHandle GraphicalPlotInterface::AddPath(std::vector<glm::vec3>& path,
					   glm::vec3 color)
{
    PathHandle p;
    return p;
}

/* Removes a path using its path handle */
void GraphicalPlotInterface::RemovePath(PathHandle pathhandle)
{
//    vertexmap.erase(pathhandle);
}

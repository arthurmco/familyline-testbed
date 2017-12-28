#include "DebugPlot.hpp"

using namespace Tribalia::Logic;


/* Adds a vector of points to form a path.
   Return a path handle */
PathHandle NullPlotInterface::AddPath(std::vector<glm::vec3>& path,
			   glm::vec3 color)
{
    (void)color;
    return (PathHandle)path[0].x;
}

/* Removes a path using its path handle */
void NullPlotInterface::RemovePath(PathHandle pathhandle)
{
    (void)pathhandle;
}

std::unique_ptr<DebugPlotInterface> DebugPlotter::interface;

void DebugPlotter::Init()
{
    interface = std::unique_ptr<DebugPlotInterface>(
	(DebugPlotInterface*) new NullPlotInterface);
}

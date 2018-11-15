#include "DebugPlot.hpp"

using namespace familyline::logic;


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

std::unique_ptr<DebugPlotInterface> DebugPlotter::pinterface;

void DebugPlotter::Init()
{
    pinterface = std::unique_ptr<DebugPlotInterface>(
	(DebugPlotInterface*) new NullPlotInterface);
}

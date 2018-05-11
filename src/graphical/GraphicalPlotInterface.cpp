#include "GraphicalPlotInterface.hpp"
#include "TerrainRenderer.hpp"
#include <functional>

using namespace Familyline::Graphics;
using namespace Familyline::Logic;

GraphicalPlotInterface::GraphicalPlotInterface(Renderer* renderer)
    : _renderer(renderer)
{}

/* Adds a vector of points to form a path.
   Return a path handle */
PathHandle GraphicalPlotInterface::AddPath(std::vector<glm::vec3>& path,
					   glm::vec3 color)
{
/* Convert the game-based coordinates in opengl based ones */
    std::vector<glm::vec3> glpos;
    std::vector<glm::vec3> glcolor;
    glpos.resize(path.size());
    glcolor.resize(path.size());
    
    std::transform(path.begin(), path.end(), glpos.begin(),
		   [](glm::vec3 point) {
		       return TerrainRenderer::GameToGraphicalSpace(point);
		   });

    std::generate(glcolor.begin(), glcolor.end(), [&color](){
	    return color;
	});

    VertexData* v = new VertexData();
    v->Positions = glpos;
    v->Normals = glcolor;
    v->render_format = VertexRenderStyle::PlotLines;
    return (PathHandle)_renderer->AddVertexData(v, new glm::mat4(1.0));
    
}

/* Removes a path using its path handle */
void GraphicalPlotInterface::RemovePath(PathHandle pathhandle)
{
    _renderer->RemoveVertexData((PathHandle)pathhandle);
}

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
/* Convert the game-based coordinates in opengl based ones */
    std::vector<glm::vec3> glpos;
    std::vector<glm::vec3> glcolor;
    glpos.resize(path.size());
    glcolor.resize(path.size());
    
    std::transform(path.begin(), path.end(), glpos.begin(),
		   [](glm::vec3 point) {
		       return GameToGraphicalSpace(point);
		   });

    std::generate(glcolor.begin(), glcolor.end(), [&color](){
	    return color;
	});

    VertexData* v = new VertexData();
    v->position = glpos;
    v->normals = glcolor;
    //v->render_format = VertexRenderStyle::PlotLines;
//    return (PathHandle)_renderer->addVertexData(v, new glm::mat4(1.0));
    VertexHandle vdata = _renderer->addVertexData(
	*v, VertexInfo{0, 0, "lines", new glm::mat4(1.0)});


    uintptr_t vcode = (uintptr_t)v;
    PathHandle p = int(vcode);
    vertexmap[p] = vdata;
}

/* Removes a path using its path handle */
void GraphicalPlotInterface::RemovePath(PathHandle pathhandle)
{
    auto vhandle = vertexmap[pathhandle];
    _renderer->removeVertexData(std::move(vhandle));

    vertexmap.erase(pathhandle);
}

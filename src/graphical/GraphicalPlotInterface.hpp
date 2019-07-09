/***
    Graphical implementation of the DebugPlotInterface

    Copyright (C) 2017 Arthur M
***/

#ifndef GRAPHICALPLOTINTERFACE_HPP
#define GRAPHICALPLOTINTERFACE_HPP

#include <map>

#include "renderer.hpp"
#include "../logic/DebugPlot.hpp"

namespace familyline::graphics {

    class GraphicalPlotInterface : public logic::DebugPlotInterface {
    private:
	Renderer* _renderer;
//	std::map<familyline::logic::PathHandle, VertexHandle> vertexmap;
	
    public:
	GraphicalPlotInterface(Renderer* renderer);

	/* Adds a vector of points to form a path.
	   Return a path handle */
	virtual logic::PathHandle AddPath(std::vector<glm::vec3>& path,
					   glm::vec3 color);

	/* Removes a path using its path handle */
	virtual void RemovePath(logic::PathHandle pathhandle);

    };

}  // familyline::graphics




#endif /* GRAPHICALPLOTINTERFACE_HPP */

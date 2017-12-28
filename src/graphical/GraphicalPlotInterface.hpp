/***
    Graphical implementation of the DebugPlotInterface

    Copyright (C) 2017 Arthur M
***/

#ifndef GRAPHICALPLOTINTERFACE_HPP
#define GRAPHICALPLOTINTERFACE_HPP

#include "Renderer.hpp"
#include "../logic/DebugPlot.hpp"

namespace Tribalia::Graphics {

    class GraphicalPlotInterface : public Logic::DebugPlotInterface {
    private:
	Renderer* _renderer;
	
    public:
	GraphicalPlotInterface(Renderer* renderer);

	/* Adds a vector of points to form a path.
	   Return a path handle */
	virtual Logic::PathHandle AddPath(std::vector<glm::vec3>& path,
					   glm::vec3 color);

	/* Removes a path using its path handle */
	virtual void RemovePath(Logic::PathHandle pathhandle);

    };

}  // Tribalia::Graphics




#endif /* GRAPHICALPLOTINTERFACE_HPP */

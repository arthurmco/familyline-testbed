/***
    Debugging lineplotter 

    Draws all of those fun lines you see

    Copyright (C) 2017 Arthur M
***/

#ifndef DEBUGPLOT_HPP
#define DEBUGPLOT_HPP

#include <vector>
#include <glm/glm.hpp>
#include <memory>

namespace Tribalia::Logic {

    typedef int PathHandle;
    
    /* Abstract interface for line plotting */
    class DebugPlotInterface {
    public:

	/* Adds a vector of points to form a path.
	   Return a path handle */
	virtual PathHandle AddPath(std::vector<glm::vec3>& path,
				   glm::vec3 color) = 0;

	/* Removes a path using its path handle */
	virtual void RemovePath(PathHandle pathhandle) = 0;

	virtual ~DebugPlotInterface() {} 
    };

    /* Null interface for line plotting */
    class NullPlotInterface : public DebugPlotInterface {
    public:

	/* Adds a vector of points to form a path.
	   Return a path handle */
	virtual PathHandle AddPath(std::vector<glm::vec3>& path,
				   glm::vec3 color);

	/* Removes a path using its path handle */
	virtual void RemovePath(PathHandle pathhandle);

	virtual ~NullPlotInterface() {} 
    };
    
    class DebugPlotter {
    public:
	static std::unique_ptr<DebugPlotInterface> interface;

	static void Init();
    };
    
};



#endif /* DEBUGPLOT_HPP */

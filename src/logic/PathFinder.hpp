/***
    Path finder class.

    Copyright (C) 2016, 2017 Arthur M

***/

#include "Terrain.hpp"
#include "ObjectManager.hpp"
#include "LocatableObject.hpp"

#include <vector>
#include <glm/glm.hpp>

#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

namespace Tribalia {
namespace Logic {

    /* The path node structure. */
    struct PathNode {
	glm::vec2 pos;
	double f;
	
	double g,h;
	double weight;
	
	PathNode* prev;
	PathNode* next;
    };

    class PathFinder {
    private:
	ObjectManager* _om;
	
	/* Pathfinder slots 
	 * Might be reused as a bitmask
	 */
	unsigned char* _pathing_slots = nullptr;
	int _mapWidth, _mapHeight;

	/* Create a path from 'from' to 'to', but reversed
	   Returns true if a path was viable, in this case there are its nodes in 'nodelist'
	   Returns false if a path wasn't viable, returns the nodelist until the most approximated location */
	bool MakePath(glm::vec2 from, glm::vec2 to, std::list<PathNode*>& nodelist);
	PathNode* CreateNode(glm::vec2 node, glm::vec2 from, glm::vec2 to);
	void CreateNeighbors(PathNode* n, std::list<PathNode*>& lopen,
			    std::list<PathNode*>& lclosed, glm::vec2 from,
			     glm::vec2 to);
	
    public:
	PathFinder(ObjectManager*);

	void InitPathmap(int w, int h);
	void UpdatePathmap(int w, int h, int x = 0, int y = 0);
	void ClearPathmap(int w, int h, int x, int y);

	
	std::vector<glm::vec2> CreatePath(LocatableObject* o, glm::vec2 destination);
    };    


    
}
}



#endif /* end of include guard: PATHFINDER_HPP */

/*
 	Manages object movement paths and update them in the game

*/

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include "LocatableObject.hpp"

#ifndef OBJECTPATHMANAGER_HPP
#define OBJECTPATHMANAGER_HPP

namespace Tribalia {
namespace Logic {

/* The object path unit */
struct ObjectPathRef {
	int pathid;
	LocatableObject* lc;
	std::vector<glm::vec2>* path;
	glm::vec2 path_point;
	int path_ptr = 0;

	ObjectPathRef(int pathid, LocatableObject* lc, 
					std::vector<glm::vec2>* path)
	{
		this->pathid = pathid;
		this->lc = lc;
		this->path = path;
		this->path_point = (*path)[0];
		this->path_ptr = 0;
	}
};

class ObjectPathManager {
private:	
	std::vector<ObjectPathRef> _pathrefs;

public:
	/* 	Adds a path from object 'o' to the path manager 
	 	Returns true if added successfully, or false if there's already a path there
		for the same object
	 */
	bool AddPath(LocatableObject* o, std::vector<glm::vec2>* path);

	/* 	Removes a path from object 'oid'. 
	 	Returns true if path removed, or false if path didn't exist there */
	bool RemovePath(long oid);

	/* Update the paths
	 * Also, removes the completed paths
	 * */
	void UpdatePaths();

	static ObjectPathManager* getInstance()	{
		static ObjectPathManager* i = nullptr;
		if (!i) 	i = new ObjectPathManager;

		return i;
	}

};

}
}

#endif

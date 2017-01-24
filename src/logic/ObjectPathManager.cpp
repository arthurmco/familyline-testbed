#include "ObjectPathManager.hpp"

int maxpathID = 0;
using namespace Tribalia::Logic;


/* 	Adds a path from object 'o' to the path manager 
Returns true if added successfully, or false if there's already a path there
	for the same object
 */
bool ObjectPathManager::AddPath(LocatableObject* o, 
				std::vector<glm::vec2>* path)
{
	for (auto& ref : _pathrefs) {
		if (ref.lc->GetObjectID() == o->GetObjectID()) {
			return false;
		}
	}

	printf("--- added path");
	_pathrefs.emplace_back(maxpathID++, o, 
					new std::vector<glm::vec2>(*path));	
	return true;
}

/* 	Removes a path from object 'oid'. 
 	Returns true if path removed, or false if path didn't exist there */
bool ObjectPathManager::RemovePath(long oid)
{
	for (auto it = _pathrefs.begin(); it != _pathrefs.end(); it++) {
		if (it->lc->GetObjectID() == oid) {
			_pathrefs.erase(it);
			return true;
		}
	}

	return false;
}

/* Update the paths
 * Also, removes the completed paths
 * */
void ObjectPathManager::UpdatePaths()
{
	/* Store a vector of 'completed path' iterators.
	 * Nuke them in iteration end */
	std::vector<std::vector<ObjectPathRef>::iterator> compl_its;

	for (auto it = _pathrefs.begin(); it != _pathrefs.end(); it++) {
		if (it->path_point == it->path->back()) {
			/* Path completed. Remove the iterator */
			compl_its.push_back(it);
		}

		printf("-> pp of %s is (%.1f, %.1f)\n",
				it->lc->GetName(), it->path_point.x, it->path_point.y);
		it->lc->SetX(it->path_point.x);
		it->lc->SetZ(it->path_point.y);

		if (it->path_ptr < it->path->size()-1)
			it->path_point = (*it->path)[++(it->path_ptr)];
	}

	/* Delete the reserved iterators */
	for (auto& it : compl_its) {
		_pathrefs.erase(it);
	}
}



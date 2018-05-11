/*
  The build queue registers items to-be built by the user.

  When you click a building, the build queue schedules its build so it can be
  built in the mouse cursor when you 'click' it again (in this case, calling
  the BuildNext() method)

  Copyright 2017 Arthur M
*/

#include "Action.hpp"
#include "LocatableObject.hpp"
#include <queue>
#include <glm/glm.hpp>

#ifndef BUILDQUEUE_HPP
#define BUILDQUEUE_HPP

namespace Familyline::Logic {
    class BuildQueue {
    private:
	std::queue<LocatableObject*> _objects;

    public:
	void Add(LocatableObject*);

	LocatableObject* BuildNext(glm::vec3 pos);
	LocatableObject* GetNext();
	void Clear();

	static BuildQueue* GetInstance() {
	    static BuildQueue* i = nullptr;
	    if (!i) i = new BuildQueue();

	    return i;
	}
    };

    bool DefaultBuildHandler(Action* ac, ActionData data, LocatableObject* built);
}


#endif //BUILDQUEUE_HPP

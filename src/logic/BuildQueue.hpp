/*
  The build queue registers items to-be built by the user.

  When you click a building, the build queue schedules its build so it can be
  built in the mouse cursor when you 'click' it again (in this case, calling
  the BuildNext() method)

  Copyright 2017 Arthur M
*/

#include "Action.hpp"
#include "AttackableObject.hpp"
#include <queue>
#include <glm/glm.hpp>

#ifndef BUILDQUEUE_HPP
#define BUILDQUEUE_HPP

namespace familyline::logic {
    class BuildQueue {
    private:
	std::queue<AttackableObject*> _objects;

    public:
	void Add(AttackableObject*);

	AttackableObject* BuildNext(glm::vec3 pos);
	AttackableObject* GetNext();
	void Clear();

	static BuildQueue* GetInstance() {
	    static BuildQueue* i = nullptr;
	    if (!i) i = new BuildQueue();

	    return i;
	}
    };

    bool DefaultBuildHandler(Action* ac, ActionData data, AttackableObject* built);
}


#endif //BUILDQUEUE_HPP

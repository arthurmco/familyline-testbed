#include "BuildQueue.hpp"
#include <Log.hpp>

using namespace familyline::logic;

void BuildQueue::Add(GameObject* o)
{
    if (!o) {
	Log::GetLog()->Warning("build-queue",
			       "adding a null pointer to the build queue!");
	return;
    }
    
    this->_objects.push(o);
}
     
GameObject* BuildQueue::BuildNext(glm::vec3 pos)
{
    auto* o = this->_objects.front();
    o->position.x = pos.x;
    o->position.z = pos.z;
    this->_objects.pop();
    return o;
}

GameObject* BuildQueue::GetNext()
{
    if (this->_objects.empty()) return nullptr;

    return this->_objects.front();
}

void BuildQueue::Clear() {
    while(!this->_objects.empty())
	this->_objects.pop();
}

bool DefaultBuildHandler(Action* ac, ActionData data, GameObject* built) {
    (void)ac;
    
    if (!built) built = data.actionOrigin;

    BuildQueue::GetInstance()->Add(built);
    return true;
}

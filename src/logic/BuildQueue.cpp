#include "BuildQueue.hpp"

using namespace Familyline::Logic;

void BuildQueue::Add(LocatableObject* o)
{
    this->_objects.push(o);
}
     
LocatableObject* BuildQueue::BuildNext(glm::vec3 pos)
{
    auto* o = this->_objects.front();
    o->SetX(pos.x);
    o->SetZ(pos.z);
    this->_objects.pop();
    return o;
}

LocatableObject* BuildQueue::GetNext()
{
    if (this->_objects.empty()) return nullptr;
	
    return this->_objects.front();
}

void BuildQueue::Clear() {
    while(!this->_objects.empty())
	this->_objects.pop();
}

bool DefaultBuildHandler(Action* ac, ActionData data, LocatableObject* built) {
    (void)ac;
    
    if (!built) built = data.actionOrigin;

    BuildQueue::GetInstance()->Add(built);
    return true;
}

#include "BuildQueue.hpp"

using namespace Familyline::Logic;

void BuildQueue::Add(AttackableObject* o)
{
    this->_objects.push(o);
}
     
AttackableObject* BuildQueue::BuildNext(glm::vec3 pos)
{
    auto* o = this->_objects.front();
    o->position.x = pos.x;
    o->position.z = pos.z;
    this->_objects.pop();
    return o;
}

AttackableObject* BuildQueue::GetNext()
{
    if (this->_objects.empty()) return nullptr;
	
    return this->_objects.front();
}

void BuildQueue::Clear() {
    while(!this->_objects.empty())
	this->_objects.pop();
}

bool DefaultBuildHandler(Action* ac, ActionData data, AttackableObject* built) {
    (void)ac;
    
    if (!built) built = data.actionOrigin;

    BuildQueue::GetInstance()->Add(built);
    return true;
}

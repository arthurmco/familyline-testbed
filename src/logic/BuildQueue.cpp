#include "BuildQueue.hpp"
#include <Log.hpp>

using namespace familyline::logic;

void BuildQueue::Add(std::shared_ptr<GameObject> o)
{
    if (o)
        this->_objects.push(o);
}
     
std::shared_ptr<GameObject> BuildQueue::BuildNext(glm::vec3 pos)
{
    auto o = this->_objects.front();

    auto opos = o->getPosition();
    opos.x = pos.x;
    opos.z = pos.z;
    o->setPosition(opos);

    this->_objects.pop();
    return o;
}

std::optional<std::shared_ptr<GameObject>> BuildQueue::getNext() const
{
    if (this->_objects.empty()) {
        return std::optional<std::shared_ptr<GameObject>>();
    } else {
        return std::make_optional(this->_objects.front());
    }
}

void BuildQueue::Clear() {
    while(!this->_objects.empty())
        this->_objects.pop();
}

bool DefaultBuildHandler(Action* ac, ActionData data, GameObject* built) {
    (void)ac;
    
    if (!built) built = data.actionOrigin;

    BuildQueue::GetInstance()->Add(std::shared_ptr<GameObject>(built));
    return true;
}

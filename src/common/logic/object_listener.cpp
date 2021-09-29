#include <common/logic/object_listener.hpp>
#include <common/logic/logic_service.hpp>

using namespace familyline::logic;

ObjectListener::ObjectListener()
{
    using namespace std::placeholders;

    LogicService::getActionQueue()->addReceiver(
        "object-listener",
        std::bind(&ObjectListener::updateObjects, this, _1),
        {
            ActionQueueEvent::Created,
            ActionQueueEvent::Destroyed,
        });

}

/**
 * Update the object statuses according to the events
 */
bool ObjectListener::updateObjects(const EntityEvent& e)
{

    if (auto* evCreate = std::get_if<EventCreated>(&e.type); evCreate) {
        this->_objects.insert(evCreate->objectID);
        return true;
    }

    if (auto* evDestroy = std::get_if<EventDestroyed>(&e.type); evDestroy) {
        this->_objects.erase(evDestroy->objectID);
        return true;
    }

    return false;
}

std::set<object_id_t> ObjectListener::getAliveObjects() const { return _objects; }


ObjectListener::~ObjectListener() { LogicService::getActionQueue()->removeReceiver("object-listener"); }

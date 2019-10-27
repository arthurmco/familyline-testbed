#include "object_listener.hpp"

using namespace familyline::logic;

/**
 * Update the object statuses according to the events
 */
void ObjectListener::updateObjects()
{
    Event e = {};

    while (this->pollEvent(e)) {
        switch (e.type) {
        case EventType::ObjectCreated:
            this->_objects.insert(e.object.id);
            break;

        case EventType::ObjectDestroyed:
            this->_objects.erase(e.object.id);
            break;
        }
        
    }
}

std::set<object_id_t> ObjectListener::getAliveObjects() const
{
    return _objects;
}

#include "logic_service.hpp"

ObjectListener::~ObjectListener()
{
    LogicService::getActionQueue()->removeReceiver(this);
}


#include "ObjectEventEmitter.hpp"

#include "ObjectEventListener.hpp"
#include "ObjectManager.hpp"

using namespace Familyline::Logic;

std::queue<ObjectEvent> ObjectEventEmitter::events;
std::vector<ObjectEventListener*> ObjectEventEmitter::listeners;

void ObjectEventEmitter::pushMessage(ObjectManager* const manager, ObjectEvent e)
{
    e.object_manager = manager;

    static const char* statusstr[] = {
	"ObjectCreated", "ObjectDestroyed"
    };

    printf("<Message %s (%#x) sent from %p to %p >\n", statusstr[e.type], e.type,
	   e.from, e.to);
    ObjectEventEmitter::events.push(e);
}

/**
 * Distribute messages to the listeners
 */
void ObjectEventEmitter::distributeMessages()
{
    while (!ObjectEventEmitter::events.empty()) {
	auto ev = ObjectEventEmitter::events.front();
	for (auto l : ObjectEventEmitter::listeners) {
	    l->pushEvent(ev);
	}
	ObjectEventEmitter::events.pop();
    }
}

void ObjectEventEmitter::addListener(ObjectEventListener* l)
{
    ObjectEventEmitter::listeners.push_back(l);
}

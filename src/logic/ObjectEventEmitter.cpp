#include "ObjectEventEmitter.hpp"

#include "ObjectEventListener.hpp"
#include "ObjectManager.hpp"

#include <Log.hpp>

using namespace Familyline::Logic;

std::queue<ObjectEvent> ObjectEventEmitter::events;
std::vector<ObjectEventListener*> ObjectEventEmitter::listeners;

void ObjectEventEmitter::pushMessage(ObjectManager* const manager, ObjectEvent e)
{
    e.object_manager = manager;

    static const char* statusstr[] = {
	"(null)", "ObjectCreated", "ObjectDestroyed"
    };

    Log::GetLog()->InfoWrite("object-event-emitter",
			     "<Message %s (%#x) sent from %p to %p >\n",
			     statusstr[e.type], e.type, e.from, e.to);
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


/**
 * Clear event queue
 */
void ObjectEventEmitter::clearEvents()
{
    while (!ObjectEventEmitter::events.empty()) {
	ObjectEventEmitter::events.pop();
    }
}

void ObjectEventEmitter::clearListeners()
{
    ObjectEventEmitter::listeners.clear();
}

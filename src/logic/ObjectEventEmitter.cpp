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
	"(null)", "ObjectCreated", "ObjectDestroyed", "ObjectCityChanged",
	"ObjectStateChanged"
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

	static const char* object_type[] =
	    {"EventNone", "ObjectCreated", "ObjectDestroyed",
	     "ObjectCityChanged", "ObjectStateChanged"};

	auto efrom = ev.from.lock();
	auto eto = ev.to.lock();
	
	Log::GetLog()->InfoWrite("object-event-emitter",
				 "event type %s (%#x) from %p (%s, %d) to %p (%s, %d)"
				 " -> %zu listeners",
				 (ev.type > ObjectStateChanged) ? "???" : object_type[ev.type],
				 ev.type,
				 ev.from, !ev.from.expired() ? efrom->getName() : "null",
				 !ev.from.expired() ? efrom->getID() : 0,
				 ev.to, !ev.to.expired() ? eto->getName() : "null",
				 !ev.to.expired() ? eto->getID() : 0,
				 ObjectEventEmitter::listeners.size());


	for (auto l : ObjectEventEmitter::listeners) {
	    l->pushEvent(ev);
	}
	ObjectEventEmitter::events.pop();
    }
}

void ObjectEventEmitter::addListener(ObjectEventListener* l)
{
    Log::GetLog()->InfoWrite("object-event-emitter", "added listener");
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

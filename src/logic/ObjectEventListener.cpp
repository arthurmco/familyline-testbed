#include "ObjectEventListener.hpp"

using namespace Familyline::Logic;

void ObjectEventListener::pushEvent(ObjectEvent e)
{
    _events.push(e);
}

bool ObjectEventListener::hasEvent()
{
    return !_events.empty();
}

/**
 * Returns true if we have an event, false if not
 * If we have, attributes the event to the variable e
 */
bool ObjectEventListener::popEvent(ObjectEvent& e)
{
    if (!_events.empty()) {
	ObjectEvent ev = _events.front();
	e = ev;
	_events.pop();
	return true;
    }
    return false;
}

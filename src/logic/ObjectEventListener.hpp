/**
 * Object event listener class
 *
 * Copyright (C) 2018 Arthur Mendes
 */

#ifndef OBJECTEVENTLISTENER_HPP
#define OBJECTEVENTLISTENER_HPP

#include <queue>

#include "ObjectEvent.hpp"

namespace familyline::logic {

    /**
     * An interface that every class that wishes to list for object events must inherit
     */
    class ObjectEventListener {
    private:
	std::queue<ObjectEvent> _events;

    public:
	void pushEvent(ObjectEvent e);

	bool hasEvent();

	/**
	 * Returns true if we have an event, false if not
	 * If we have, attributes the event to the variable e
	 */
	bool popEvent(ObjectEvent& e);
    };


    

}  // familyline::logic


#endif /* OBJECTEVENTLISTENER_HPP */

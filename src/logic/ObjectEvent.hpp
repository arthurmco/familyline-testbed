/**
 * Object event structure definitions
 *
 * This structure is the base for object event system
 *
 * Copyright (C) 2018 Arthur Mendes
 */

#ifndef OBJECTEVENT_HPP
#define OBJECTEVENT_HPP

#include "GameObject.hpp"

namespace Familyline::Logic {

    class ObjectManager;
    class City;
    
    enum ObjectEventType {

	// A null event
	None,
	
	// An object has been created
	ObjectCreated,

	// An object has been destroyed
	ObjectDestroyed,

	// An object changed its city
	ObjectCityChanged,
    };


/* An object event. Haha */
    struct ObjectEvent {
	const ObjectManager* object_manager;
	const GameObject* from;
	const GameObject* to;

	ObjectEventType type;

	union {
	    // for ObjectCreated and ObjectDestroyed
	    struct {
		int oid = 0;
	    };

	    // for ObjectCityChanged
	    struct {
		const City* city;
	    };
	};

	ObjectEvent();
	ObjectEvent(const GameObject* from, const GameObject* to, ObjectEventType type);
	ObjectEvent(const GameObject* from, City* city);
    };

}
    
#endif /* OBJECTEVENT_HPP */

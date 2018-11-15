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

namespace familyline::logic {

    class ObjectManager;
    class City;
    
    enum ObjectEventType {

	// A null event
	EventNone,
	
	// An object has been created
	ObjectCreated,

	// An object has been destroyed
	ObjectDestroyed,

	// An object changed its city
	ObjectCityChanged,

	// An object changed state
	ObjectStateChanged,
    };


/* An object event. Haha */
    struct ObjectEvent {
	const ObjectManager* object_manager;

	// Destroyed: The destroyed object
	// StateChanged: the object that changed state
	std::weak_ptr<GameObject> from;

	// Created: The created object
	std::weak_ptr<GameObject> to;

	ObjectEventType type;

	union {
	    // for ObjectCreated and ObjectDestroyed
	    struct {
		int oid;
	    };

	    // for ObjectCityChanged
	    struct {
		const City* city;
	    };

	    // for ObjctStateChanged
	    struct {
		ObjectState oldstate;
		ObjectState newstate;
	    };
	};

	ObjectEvent();
	ObjectEvent(std::weak_ptr<GameObject> from, std::weak_ptr<GameObject> to, ObjectEventType type);
	ObjectEvent(std::weak_ptr<GameObject> from, City* city);
	ObjectEvent(std::weak_ptr<GameObject> from, ObjectState oldstate, ObjectState newstate);
    };

}
    
#endif /* OBJECTEVENT_HPP */

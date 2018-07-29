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

    enum ObjectEventType {

	// An object has been created
	ObjectCreated,

	// An object has been destroyed
	ObjectDestroyed,

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
	};

	ObjectEvent(const GameObject* from, const GameObject* to, ObjectEventType type)
	    : from(from), to(to), type(type)
	    {}

    };

}
    
#endif /* OBJECTEVENT_HPP */

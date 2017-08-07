/*  
    Action information

    Copyright 2016, 2017 Arthur M
*/

#include <functional>
#include <cstddef>
#include <cstring>
#include "LocatableObject.hpp"

#ifndef ACTION_HPP
#define ACTION_HPP

namespace Tribalia {
namespace Logic {

    /* An action is any predefined command done by an object.
       Examples are: train unit, build construction, repair...

       They usually are done by the player, but the commands can be invoked
       from the entity
    */
   
    struct Action;

    /* Action event data */
    struct ActionData {
	/* Player positional data.
	   Mostly used for building objencts */
	float xPos, yPos;

	/* The object that registered the action */
	LocatableObject *actionOrigin;
    };
    
    /* Represents an action handler
       It's run each time an action is invoked

       An action might be invoked by two ways:
        - in the user interface
	- manually invoking it in the ActionManager

       Returns true if handler has been accepted, false if not (e.g you 
       have insufficient money to run the action)
    */
    typedef std::function<bool(Action* ac, ActionData acdata)> ActionHandler;
    
    /* Represents an individual action */
    struct Action {
	char* name;
	char* assetname;
	ActionHandler handler = nullptr;
	size_t refcount = 1;

	Action(const char* name, const char* assetname)
	    : name((char*)name), assetname((char*)assetname) {}
	
	Action()
	    : Action("", "") {}

	~Action() {
	    if (refcount == 0) {
		delete name;
		delete assetname;
	    }
	}

	Action(const Action& other)
	: handler(other.handler), refcount(1) {

	    char *nname = nullptr, *nasset = nullptr;

	    if (other.name) {
		nname = new char[strlen(other.name)+1];
		strcpy(nname, other.name);
	    }

	    if (other.assetname) {
		nasset = new char[strlen(other.assetname)+1];
		strcpy(nasset, other.assetname);
	    }

	    this->name = nname;
	    this->assetname = nasset;
	    this->handler = other.handler;
	    this->refcount = 1;
	}

	
	Action& operator=(const Action& other){

	    char *nname = nullptr, *nasset = nullptr;

	    if (other.name) {
		nname = new char[strlen(other.name)+1];
		strcpy(nname, other.name);
	    }

	    if (other.assetname) {
		nasset = new char[strlen(other.assetname)+1];
		strcpy(nasset, other.assetname);
	    }

	    this->name = nname;
	    this->assetname = nasset;
	    this->handler = other.handler;
	    this->refcount = 1;
	    return *this;
	}

	Action& operator=(const Action&& other) {

	    if (this == &other)
		return *this;

	    char *nname = nullptr, *nasset = nullptr;

	    if (other.name) {
		nname = new char[strlen(other.name)+1];
		strcpy(nname, other.name);
		delete other.name;
	    }

	    if (other.assetname) {
		nasset = new char[strlen(other.assetname)+1];
		strcpy(nasset, other.assetname);
		delete other.assetname;
	    }

	    this->name = nname;
	    this->assetname = nasset;
	    this->handler = other.handler;
	    this->refcount = other.refcount;
	    return *this;
	    
	}
	
	Action(const Action&& other)
	    : handler(other.handler), refcount(other.refcount) {

	    if (this == &other)
		return;

	    char *nname = nullptr, *nasset = nullptr;

	    if (other.name) {
		nname = new char[strlen(other.name)+1];
		strcpy(nname, other.name);
		delete other.name;
	    }

	    if (other.assetname) {
		nasset = new char[strlen(other.assetname)+1];
		strcpy(nasset, other.assetname);
		delete other.assetname;
	    }

	    this->name = nname;
	    this->assetname = nasset;
	}



	      
    };
    
}
}




#endif /* end of include guard: ACTION_HPP */

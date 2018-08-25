/*	Represents an object factory
 
	Copyright (C) 2016, 2018 Arthur M
*/

#ifndef OBJECTFACTORY_HPP
#define OBJECTFACTORY_HPP


#include <map>
#include <string>

#include "GameObject.hpp"

namespace Familyline::Logic {
	
class ObjectFactory {
private:
    std::map<std::string /*type*/, GameObject*> _objects;

public:
	/* Gets an instance of object with type 'typeID', or
	 * nullptr if given object wasn't added to the factory
	 * i.e, doesn't exist */
    GameObject* GetObject(const char* type, float x, float y, float z);

    /* Adds an object to the factory */
    void AddObject(GameObject* object);

    static ObjectFactory* GetInstance() {
	static ObjectFactory* f = nullptr;
	if (!f) 	f = new ObjectFactory;
	
	return f;
    }
};

}

#endif
 

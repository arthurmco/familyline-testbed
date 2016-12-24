#include "ObjectFactory.hpp"

using namespace Tribalia::Logic;

/* Gets an instance of object with type 'typeID', or
 * nullptr if given object wasn't added to the factory
 * i.e, doesn't exist */
GameObject* ObjectFactory::GetObject(int typeID)
{
	if (_objects.find(typeID) != _objects.end()) {
		return _objects[typeID];
	}	
	
	return nullptr;
}

/* Adds an object to the factory */
void ObjectFactory::AddObject(GameObject* object)
{
	_objects[object->GetTypeID()] = object;
}

#include "ObjectFactory.hpp"

using namespace Familyline::Logic;

/* Gets an instance of object with type 'typeID', or
 * nullptr if given object wasn't added to the factory
 * i.e, doesn't exist */
GameObject* ObjectFactory::GetObject(int typeID, float x, float y, float z)
{
	// if (_objects.find(typeID) != _objects.end()) {
	//     const char* n = _objects[typeID]->GetName();
	//     char* nn = new char[strlen(n)+1];
	//     memcpy(nn, n, strlen(n)+1);
	//     return _objects[typeID]->Clone(x, y, z, nn);
	// }	
	
    return nullptr;
}

/* Adds an object to the factory */
void ObjectFactory::AddObject(GameObject* object)
{
//	_objects[object->GetTypeID()] = object;
}

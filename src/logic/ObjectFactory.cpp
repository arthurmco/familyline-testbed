#include "ObjectFactory.hpp"

using namespace Familyline::Logic;

/* Gets an instance of object with type 'type', or
 * nullptr if given object wasn't added to the factory
 * i.e, doesn't exist */
GameObject* ObjectFactory::GetObject(const char* type, float x, float y, float z)
{
    if (_objects.find(type) != _objects.end()) {
//	const char* n = _objects[type]->GetName();
//	char* nn = new char[strlen(n)+1];
//	memcpy(nn, n, strlen(n)+1);
//	return _objects[type]->Clone(x, y, z, nn);
	return _objects[type]->clone(glm::vec3(x, y, z));
    }	

    return nullptr;
}

/* Adds an object to the factory */
void ObjectFactory::AddObject(GameObject* object)
{
    _objects[object->getType()] = object;
}

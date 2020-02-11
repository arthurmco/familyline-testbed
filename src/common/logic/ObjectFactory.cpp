#include <common/logic/ObjectFactory.hpp>

using namespace familyline::logic;

/* Gets an instance of object with type 'type', or
 * nullptr if given object wasn't added to the factory
 * i.e, doesn't exist */
std::shared_ptr<GameObject> ObjectFactory::GetObject(const char* type, float x, float y, float z)
{
    if (_objects.find(type) != _objects.end()) {
        //  const char* n = _objects[type]->GetName();
        //  char* nn = new char[strlen(n)+1];
        //  memcpy(nn, n, strlen(n)+1);
        //  return _objects[type]->Clone(x, y, z, nn);
        auto newo = _objects[type]->create();
        newo->setPosition(glm::vec3(x, y, z));
        return newo;
    }   

    return nullptr;
}

/* Adds an object to the factory */
void ObjectFactory::AddObject(GameObject* object)
{
    _objects[object->getType()] = object;
}

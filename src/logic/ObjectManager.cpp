#include "ObjectManager.hpp"

#include "ObjectEvent.hpp"
#include "ObjectEventEmitter.hpp"
#include <Log.hpp>

using namespace Familyline;
using namespace Familyline::Logic;

/**
 * Add the object 'o' to the object manager and give to it a new ID
 *
 * Returns a pointer to the object.
 * The object manager needs to be the owner of the object, so it
 * will store a unique_ptr of it
 */
GameObject* ObjectManager::addObject(GameObject*&& o) {
    auto oid = ++last_id;
    o->id = oid;
    o->gam = this;

    objects[oid] = std::unique_ptr<GameObject>(o);
    GameObject* obj = this->getObject(oid);

    // Generate the event
    ObjectEvent e(nullptr, obj, ObjectEventType::ObjectCreated);
    e.oid = oid;
    ObjectEventEmitter::pushMessage(this, e);

    return obj;
}


/**
 * Removes the object 'o' from the object manager
 *
 * We, then, delete it
 */
void ObjectManager::removeObject(const GameObject* o) {

    auto obj = objects.find(o->getID());

    if (obj != objects.end()) {
	auto id = obj->second->getID();

	// Generate the event
	ObjectEvent e(o, nullptr, ObjectEventType::ObjectDestroyed);
	ObjectEventEmitter::pushMessage(this, e);

	obj->second.reset();
	this->objects.erase(id);
    }
}

/**
 * Gets an object from the list, by ID
 *
 * Retrieves a weak pointer to the object.
 */
GameObject* ObjectManager::getObject(object_id_t id) {

    auto obj = objects.find(id);
    if (obj != objects.end()) {
	return obj->second.get();
    } else {
	return nullptr;
    }
}

void ObjectManager::iterateAll() {
    for (auto& go : this->objects)
	go.second->iterate();
}

/**
 * Destroy all objects owned by this object manager
 */
ObjectManager::~ObjectManager() {
    for (auto& o : objects) {
	o.second.reset();
    }
}

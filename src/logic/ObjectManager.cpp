#include "ObjectManager.hpp"

#include "ObjectEvent.hpp"
#include "ObjectEventEmitter.hpp"
#include <Log.hpp>

using namespace familyline;
using namespace familyline::logic;

ObjectManager* ObjectManager::instance;


/**
 * Add the object 'o' to the object manager and give to it a new ID
 *
 * Returns a weak pointer to the object.
 * The object manager needs to be the owner of the object, so it
 * will store the original, shared version of it
 *
 * You can lock if if you want to use
 */
std::weak_ptr<GameObject> ObjectManager::addObject(GameObject*&& o) {
    auto oid = ++last_id;
    o->id = oid;
    o->gam = this;

    objects[oid] = std::shared_ptr<GameObject>(o);
    auto obj = this->getObject(oid);

    // Generate the event
    ObjectEvent e(std::weak_ptr<GameObject>(), obj, ObjectEventType::ObjectCreated);
    e.oid = oid;
    ObjectEventEmitter::pushMessage(this, e);

    return obj;
}

/**
 * Removes the object 'o' from the object manager
 *
 * It will expire after the last reference to it is destroyed
 */
void ObjectManager::removeObject(std::shared_ptr<GameObject> o) {

    auto obj = objects.find(o->getID());

    if (obj != objects.end()) {
	auto id = obj->second->getID();

	// Generate the event
	ObjectEvent e(o, std::weak_ptr<GameObject>(), ObjectEventType::ObjectDestroyed);
	e.oid = o->getID();
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
std::weak_ptr<GameObject> ObjectManager::getObject(object_id_t id) {

    auto obj = objects.find(id);
    if (obj != objects.end()) {
	std::weak_ptr<GameObject> wobj = obj->second;
	return wobj;
    } else {
	return std::weak_ptr<GameObject>(); // returns an empty ptr
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

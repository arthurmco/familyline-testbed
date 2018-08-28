
/***
    Game object class definition

    Copyright 2016, 2018 Arthur Mendes.

***/
#include "GameObject.hpp"
#include "ObjectManager.hpp"
#include <cstring>

using namespace Familyline::Logic;

/**
 * Clears all references in the reference hashmap
 *
 * This is done so we can't have 'stale references', they are cleared in every iteration
 *
 * (TODO: maybe clear them in each n iterations
 */
void GameObject::clearReferences()
{
    references.clear();
}

/**
 * Gets an object and stores the gotten object in a place
 *
 * This function is good to know what objects are being referenced to this one and what ones
 * do this one references. So, for example, we can execute the iterate() method in a correct
 * order, from the more referenced ones to the least referenced.
 *
 * You should not use getObject directly while in an object, but getObjectReference instead
 */
const GameObject* GameObject::getObjectReference(object_id_t id)
{
    auto o = gam->getObject(id);
    this->references[id] = true;
    return o.lock().get();
}

/**
 * Check if the object 'other' collided with the actual object
 *
 * Since we have some objects that aren't exactly convex, like a wall or a road, or
 * objects that has multiple individual locations, like a city, needs to have a custom
 * collision algorithm
 *
 * For the basic objects, we'll use a simple point-to-cube collision detection. The object
 * will be treated like a cube, with 'radius'*2 size
 *
 * If you need, you can subclass this class
 */
bool GameObject::hasCollided(GameObject* const other)
{
    (void)other;

    return false;
}

/**
 * Function with code that the object must run on every engine tick
 *
 * In it you put actions that the object must do
 */
void GameObject::iterate()
{
    this->clearReferences();
}

/* Clone the object at a specified position
 * 
 * This function needs to be implemented for each object, because it
 * will return the same class as the object
 *
 * This is used in the ObjectFactory, to clone the object
 *
 * Returns a new object
 */
GameObject* GameObject::clone(glm::vec3 position)
{
    auto a = new GameObject(this->id, this->type, this->name);
    a->position = position;
    return a;
}

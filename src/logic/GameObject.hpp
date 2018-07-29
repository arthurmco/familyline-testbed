
/***
    Game object class implementation

    Copyright (C) 2016-2018 Arthur Mendes.

***/

#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <memory>
#include <unordered_map>

#include "IMesh.hpp"

namespace Familyline::Logic {

    class ObjectManager;

    /**
     * Object ID type
     */
    typedef int object_id_t;

    /**
     * Game object base class
     *
     * Anything into the game that has a name, size and position is inherited from this
     * class
     */

    class GameObject {
	friend class ObjectManager;
    private:

	/**
	 * The GameObjectManager we'll use for the getObjectReference() method
	 *
	 * The GameObjectManager will set itself to this field, but it's friends with this one,
	 * so we don't need a method
	 */
	ObjectManager* gam;

	/**
	 * Object reference map
	 */
	std::unordered_map<object_id_t, bool> references;

    protected:
	object_id_t id;
	const char* type;
	const char* name;

	float radius;


	/**
	 * Clears all references in the reference hashmap
	 *
	 * This is done so we can't have 'stale references', they are cleared in every iteration
	 *
	 * (TODO: maybe clear them in each n iterations
	 */
	void clearReferences();

	/**
	 * Gets an object and stores the gotten object in a place
	 *
	 * This function is good to know what objects are being referenced to this one and what ones
	 * do this one references. So, for example, we can execute the iterate() method in a correct
	 * order, from the more referenced ones to the least referenced.
	 *
	 * You should not use getObject directly while in an object, but getObjectReference instead
	 */
	const GameObject* getObjectReference(object_id_t id);

    public:
	GameObject(object_id_t id, const char* type, const char* name)
	    : id(id), type(type), name(name)
	    {}

	object_id_t getID() const { return this->id; }
	const char* getType() const { return this->type; }
	const char* getName() const { return this->name; }

	/**
	 * The object position.
	 * For 'positionless' objects, it's (-1, -1, -1)
	 */
	glm::vec3 position;

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
	bool hasCollided(GameObject* const other);
	

	/* Shared pointer to a mesh */
	std::shared_ptr<IMesh> mesh;

	// TODO: separate mesh into vertex data and transformations?

	/**
	 * Function with code that the object must run on every engine tick
	 *
	 * In it you put actions that the object must do
	 */
	virtual void iterate();


	virtual ~GameObject() {}
    };


}


#endif /* end of include guard: GAMEOBJECT_HPP */

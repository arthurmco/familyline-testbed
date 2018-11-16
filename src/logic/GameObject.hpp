/***
    Game object class implementation

    Copyright (C) 2016-2018 Arthur Mendes.

***/

#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <memory>
#include <unordered_map>

#include "IMesh.hpp"

namespace familyline::logic {

    class ObjectManager;

    /**
     * Object ID type
     */
    typedef int object_id_t;

    /**
     * Object lifecycle states
     */
    enum ObjectState {
	/**
	 * An object is being created
	 * The building animation is playing
	 */
	Creating,

	/**
	 * An object is created. The default state
	 */
	Created,

	/**
	 * An object is being attacked and is losing health
	 */
	Hurt,

	/**
	 * An object has just lost all of its lifepoints,
	 * The death animation is playing
	 */
	Dying,

	/**
	 * An object is dead and will be destroyed soon 
	 */
	Dead
    };

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
	 * \warning You should not use getObject directly while in an object, 
	 *          but getObjectReference instead
	 */
	const GameObject* getObjectReference(object_id_t id);

	/**
	 * The current stage of the object
	 */
	ObjectState state = ObjectState::Created;

    public:
	GameObject(object_id_t id, const char* type, const char* name)
	    : id(id), type(type), name(name)
	    {}

	object_id_t getID() const { return this->id; }
	const char* getType() const { return this->type; }
	const char* getName() const { return this->name; }
	ObjectState getState() const { return this->state; }

	ObjectState setState(ObjectState newstate) {
	    auto s = this->state;
	    this->state = newstate;
	    return s;
	}


	/**
	 * The object position.
	 * For 'positionless' objects, it's (-1, -1, -1)
	 */
	glm::vec3 position = glm::vec3(-1, -1, -1);

	/**
	 * The rotation
	 *
	 * The angle said here is the object rotation, the rotation alongsize Y axis
	 * We'll follow the unit circle, so a building with rotation 0deg will be facing
	 * +X axis, 90deg will be facing +Z axis, etc (tbe terrain plane Y axis is 3D Z axis)
	 *
	 * The rotation is in radians, though. It's easier for math
	 * Rotationless object should have rotation=0
	 */
	double rotation = 0;

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

	/**
	 * Clone the object at a specified position
	 *
	 * This function needs to be implemented for each object, because it
	 * will return the same class as the object
	 *
	 * This is used in the ObjectFactory, to clone the object
	 *
	 * Returns a new object
	 */
	virtual GameObject* clone(glm::vec3 position);


	virtual ~GameObject() {}
    };


}


#endif /* end of include guard: GAMEOBJECT_HPP */


/***
    Object manager class

    Copyright (C) 2016, 2018 Arthur M

***/

#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP

#include <list>
#include <vector>
#include <map>

#include "GameObject.hpp"
#include "Log.hpp"

namespace familyline::logic {

    /*
     * Manages the objects creation and deletion.
     * Also generates the events for the object event manager
     */
    class ObjectManager {

    private:
	/* The map of objects
	 * We use a map because it do perform faster than lists and vectors
	 */
	std::map<object_id_t, std::shared_ptr<GameObject>> objects;

	/* Fast reference to the last ID used */
	object_id_t last_id = 0;

	static ObjectManager* instance;
    public:
	/**
	 * Add the object 'o' to the object manager and give to it a new ID
	 *
	 * Returns a weak pointer to the object.
	 * The object manager needs to be the owner of the object, so it
	 * will store the original, shared version of it
	 *
	 * You can lock if if you want to use
	 */
	std::weak_ptr<GameObject> addObject(GameObject*&& o);

	/**
	 * Removes the object 'o' from the object manager
	 *
	 * It will expire after the last reference to it is destroyed
	 */
	void removeObject(std::shared_ptr<GameObject> o);

	/**
	 * Gets an object from the list, by ID
	 *
	 * Retrieves a weak pointer to the object.
	 */
	std::weak_ptr<GameObject> getObject(object_id_t id);

	size_t getObjectCount() { return objects.size(); }

	static ObjectManager* getDefault() { return ObjectManager::instance; }
	static void setDefault(ObjectManager* i) { ObjectManager::instance = i; }


	void iterateAll();

	/**
	 * Destroy all objects owned by this object manager
	 */
	~ObjectManager();
    };


}



#endif /* end of include guard: OBJECTMANAGER_HPP */

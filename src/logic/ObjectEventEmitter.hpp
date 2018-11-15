/**
 * Central hub for object events
 *
 * Copyright (C) 2018 Arthur Mendes
 */

#ifndef OBJECTEVENTEMITTER_HPP
#define OBJECTEVENTEMITTER_HPP

#include <queue>
#include <vector>

#include "ObjectEvent.hpp"

namespace familyline::logic {

    class ObjectEventListener;
    class ObjectManager;

    /**
     * Sends object events to the listeners
     *
     * Fully static so it can be used anywhere
     */
    class ObjectEventEmitter {
    private:
	static std::queue<ObjectEvent> events;
	static std::vector<ObjectEventListener*> listeners;

    public:
	static void pushMessage(ObjectManager* const manager, ObjectEvent e);

	/**
	 * Distribute messages to the listeners
	 */
	static void distributeMessages();

	static void addListener(ObjectEventListener* l);

	/**
	 * Clear event queue
	 */
	static void clearEvents();
	
	static void clearListeners();

    };




}  // familyline::logic




#endif /* OBJECTEVENTEMITTER_HPP */

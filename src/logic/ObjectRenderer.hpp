/***
	Object drawing class

	A bridge between the logical and the physical renderer

	Copyright (C) 2016, 2018 Arthur M

***/
#include <list>
#include <vector>
#include <map>
#include <memory>

#include "AttackableObject.hpp"
#include "ObjectManager.hpp"
#include "ObjectEventListener.hpp"
#include "ObjectEventEmitter.hpp"
#include "GameActionListener.hpp"

#include "../graphical/Mesh.hpp"
#include "../graphical/SceneManager.hpp"

#ifndef OBJECTRENDERER_HPP
#define OBJECTRENDERER_HPP

namespace Familyline::Logic {

    struct ObjectRenderData {
	int ID;
	std::shared_ptr<Familyline::Graphics::Mesh> m;
    };

    /*
      A game is fully controlled by inputs, being from player, AI or network

      The ínput ends in the game action listeners.

      Since everything happens by player action, the object rendering should
      happen by it too.
    */

    class ObjectRenderer : public GameActionListener
    {
    private:
	ObjectManager * _om;
	Familyline::Graphics::SceneManager* _sm;
	ObjectEventListener oel;

	std::vector<const GameObject*> _objects;
	std::vector<ObjectRenderData> _IDs;

	std::map<int /*id*/, bool /*exists*/> _modified_ids;
	std::map<int /*id*/, bool /*exists*/> _created_ids;
	std::map<int, bool> _deleted_ids;

    public:
	ObjectRenderer(ObjectManager*, Familyline::Graphics::SceneManager*);

	virtual void OnListen(GameAction& a);

	/*
	  Check for new and deleted objects, add them to the list
	  Return true if we have object changes, false if we haven't
	*/
	bool Check();

	/* Update object meshes */
	void Update();

    };

} /* Familyline */


#endif /* end of include guard: OBJECTRENDERER_HPP */

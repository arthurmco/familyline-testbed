/***
    Object drawing class

    A bridge between the logical and the physical renderer

    Copyright (C) 2016 Arthur M

***/
#include <list>
#include <vector>
#include <map>

#include "LocatableObject.hpp"
#include "ObjectManager.hpp"
#include "GameActionListener.hpp"

#include "../graphical/TerrainRenderer.hpp"
#include "../graphical/SceneManager.hpp"

#ifndef OBJECTRENDERER_HPP
#define OBJECTRENDERER_HPP

namespace Tribalia {
namespace Logic {

    struct ObjectRenderData {
        int ID;
        Tribalia::Graphics::Mesh* m;
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
        ObjectManager* _om;
        Tribalia::Graphics::SceneManager* _sm;

        std::list<LocatableObject*> _objects;
        std::vector<ObjectRenderData> _IDs;

	std::map<int /*id*/, bool /*exists*/> _modified_ids;
	std::map<int /*id*/, bool /*exists*/> _created_ids;
	std::map<int, bool> _deleted_ids;

    public:
        ObjectRenderer(ObjectManager*, Tribalia::Graphics::SceneManager*);

	virtual void OnListen(GameAction& a);
	
        /*	
	  Check for new and deleted objects, add them to the list 
	  Return true if we have object changes, false if we haven't
	*/
        bool Check();

        /* Update object meshes */
        void Update();

    };

}
} /* Tribalia */


#endif /* end of include guard: OBJECTRENDERER_HPP */

/***
    Definitions for human player

    Copyright (C) 2016 Arthur M

***/

#ifndef HUMAN_PLAYER
#define HUMAN_PLAYER


#ifndef __gl_h_
#include <GL/glew.h>
#endif

#include <memory>

#include <common/logic/Player.hpp>
#include <common/logic/PathFinder.hpp>
#include <common/logic/ObjectFactory.hpp>
#include <common/logic/object_manager.hpp>
#include <common/logic/ObjectPathManager.hpp>
#include <common/logic/BuildQueue.hpp>
#include <common/logic/attack_manager.hpp>
#include <common/logic/lifecycle_manager.hpp>

#include <client/graphical/camera.hpp>
#include <client/graphical/TerrainRenderer.hpp>
//#include "graphical/GUIActionManager.hpp>
#include <client/graphical/object_renderer.hpp>

#include <client/input/InputPicker.hpp>
#include <client/input/InputManager.hpp>

class HumanPlayer : public familyline::logic::Player
{
private:
    familyline::graphics::Camera* _cam;
	familyline::input::InputPicker* _ip;

	std::weak_ptr<familyline::logic::GameObject> _selected_obj;
    familyline::logic::PathFinder* _pf = nullptr;
	
    bool _updated = false;

public:
	bool renderBBs = false;

    familyline::graphics::ObjectRenderer* objr;
    familyline::logic::ObjectLifecycleManager* olm;
    
    HumanPlayer(const char* name, int xp=0,
                familyline::logic::GameActionManager* gam = nullptr);


    /***
        Virtual function called on each iteration.

        It allows player to decide its movement
        (input for humans, AI decisions for AI... )

        Returns true to continue its loop, false otherwise.
    ***/
    virtual bool Play(familyline::logic::GameContext*);

	
	/***
	    Process player input.
	    Returns true if some input was received, false if not 
	***/
	virtual bool ProcessInput();
	
	
    void SetCamera(familyline::graphics::Camera*);
	void SetPicker(familyline::input::InputPicker* ip);
    void SetInputManager(familyline::input::InputManager*);
    void SetPathfinder(familyline::logic::PathFinder*);
	void SetGameActionManager(familyline::logic::GameActionManager* );

    virtual bool HasUpdatedObject();

    familyline::logic::GameObject* GetSelectedObject();
    virtual ~HumanPlayer();
};

#endif /* end of include guard: HUMAN_PLAYER */

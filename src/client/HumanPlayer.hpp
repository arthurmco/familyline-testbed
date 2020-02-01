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

#include "logic/Player.hpp"
#include "logic/PathFinder.hpp"
#include "logic/ObjectFactory.hpp"
#include "logic/object_manager.hpp"
#include "logic/ObjectPathManager.hpp"
#include "logic/BuildQueue.hpp"
#include "logic/attack_manager.hpp"
#include "logic/lifecycle_manager.hpp"

#include "graphical/camera.hpp"
#include "graphical/TerrainRenderer.hpp"
//#include "graphical/GUIActionManager.hpp"
#include "graphical/object_renderer.hpp"

#include "input/InputPicker.hpp"
#include "input/InputManager.hpp"

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

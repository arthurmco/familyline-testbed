/***
    Definitions for human player

    Copyright (C) 2016 Arthur M

***/

#ifndef HUMAN_PLAYER
#define HUMAN_PLAYER


#ifndef __gl_h_
#include <GL/glew.h>
#endif

#include "logic/Player.hpp"
#include "logic/PathFinder.hpp"
#include "logic/ObjectFactory.hpp"
#include "graphical/Camera.hpp"
#include "graphical/TerrainRenderer.hpp"
//#include "graphical/GUIActionManager.hpp"
#include "logic/ObjectRenderer.hpp"
#include "logic/ObjectPathManager.hpp"
#include "logic/BuildQueue.hpp"
#include "logic/CombatManager.hpp"
#include "input/InputPicker.hpp"
#include "input/InputManager.hpp"

    class HumanPlayer : public Familyline::Logic::Player
    {
    private:
        Familyline::Graphics::Camera* _cam;
	Familyline::Input::InputPicker* _ip;

        Familyline::Logic::LocatableObject* _selected_obj = nullptr;
        Familyline::Logic::PathFinder* _pf = nullptr;
	
        bool _updated = false;

    public:
	bool renderBBs = false;

        Familyline::Logic::ObjectRenderer* objr;

        HumanPlayer(const char* name, int xp=0,
		    Familyline::Logic::GameActionManager* gam = nullptr);


        /***
            Virtual function called on each iteration.

            It allows player to decide its movement
            (input for humans, AI decisions for AI... )

            Returns true to continue its loop, false otherwise.
        ***/
        virtual bool Play(Familyline::Logic::GameContext*);

	
	/***
	    Process player input.
	    Returns true if some input was received, false if not 
	***/
	virtual bool ProcessInput();
	
	
        void SetCamera(Familyline::Graphics::Camera*);
	void SetPicker(Familyline::Input::InputPicker* ip);
        void SetInputManager(Familyline::Input::InputManager*);
        void SetPathfinder(Familyline::Logic::PathFinder*);
	void SetGameActionManager(Familyline::Logic::GameActionManager* );

        virtual bool HasUpdatedObject();

        Familyline::Logic::LocatableObject* GetSelectedObject();
        virtual ~HumanPlayer();
    };

#endif /* end of include guard: HUMAN_PLAYER */

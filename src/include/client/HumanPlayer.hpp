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

#include <common/logic/player.hpp>
#include <common/logic/PathFinder.hpp>
#include <common/logic/object_factory.hpp>
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
#include <client/input/input_manager.hpp>
#include <client/input/input_service.hpp>

class HumanPlayer : public familyline::logic::Player
{
private:
    familyline::graphics::Camera* _cam;
	familyline::input::InputPicker* _ip;

	std::weak_ptr<familyline::logic::GameObject> _selected_obj;
    familyline::logic::PathFinder* _pf = nullptr;

    familyline::input::HumanListenerHandler _listener;

    
    bool _updated = false;

public:
	bool renderBBs = false;

    familyline::graphics::ObjectRenderer* objr;
    familyline::logic::ObjectLifecycleManager* olm;

    HumanPlayer(familyline::logic::PlayerManager &pm, const char *name, int code);

    /**
     * Generate the input actions.
     *
     * They must be pushed to the input manager
     */
    virtual void generateInput();


	
    void SetCamera(familyline::graphics::Camera*);
	void SetPicker(familyline::input::InputPicker* ip);
    void SetInputManager(familyline::input::InputManager*);
    void SetPathfinder(familyline::logic::PathFinder*);
//	void SetGameActionManager(familyline::logic::GameActionManager* );

    familyline::logic::GameObject* GetSelectedObject();
    virtual ~HumanPlayer();
};

#endif /* end of include guard: HUMAN_PLAYER */

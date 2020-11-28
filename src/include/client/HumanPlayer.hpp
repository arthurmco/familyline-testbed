/***
    Definitions for human player

    Copyright (C) 2016 Arthur M

***/

#ifndef HUMAN_PLAYER
#define HUMAN_PLAYER

#ifndef __gl_h_
#include <GL/glew.h>
#endif

#include <client/graphical/camera.hpp>
#include <client/graphical/terrain_renderer.hpp>
#include <common/logic/BuildQueue.hpp>
#include <common/logic/attack_manager.hpp>
#include <common/logic/object_factory.hpp>
#include <common/logic/object_manager.hpp>
#include <common/logic/player.hpp>
#include <memory>
//#include "graphical/GUIActionManager.hpp>
#include <client/graphical/object_renderer.hpp>
#include <client/input/InputPicker.hpp>
#include <client/input/input_manager.hpp>
#include <client/input/input_service.hpp>

class HumanPlayer : public familyline::logic::Player
{
private:
    familyline::input::InputPicker* _ip;

    std::weak_ptr<familyline::logic::GameObject> _selected_obj;

    familyline::input::HumanListenerHandler _listener;

    std::string nextBuild_;
    
    bool _updated = false;

public:
    bool renderBBs = false;

    HumanPlayer(
        familyline::logic::PlayerManager& pm, const familyline::logic::Terrain& t, const char* name,
        int code);

    /**
     * Generate the input actions.
     *
     * They must be pushed to the input manager
     */
    virtual void generateInput();

    /**
     * Does this player requested game exit?
     */
    virtual bool exitRequested();

    void setCamera(familyline::graphics::Camera*);
    void SetPicker(familyline::input::InputPicker* ip);
    void SetInputManager(familyline::input::InputManager*) {};
    //	void SetGameActionManager(familyline::logic::GameActionManager* );

    virtual ~HumanPlayer();
};

#endif /* end of include guard: HUMAN_PLAYER */

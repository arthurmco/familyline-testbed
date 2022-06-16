#pragma once
/***
    Definitions for human player

    Copyright (C) 2016 Arthur M

***/

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
#include <client/input/command_table.hpp>
#include <client/input/input_manager.hpp>
#include <client/input/input_service.hpp>
#include <client/preview_renderer.hpp>

class HumanPlayer : public familyline::logic::Player
{
private:
    familyline::input::InputPicker* _ip;

    std::weak_ptr<familyline::logic::GameEntity> _selected_obj;

    familyline::input::HumanListenerHandler _listener;

    std::string nextBuild_;

    std::weak_ptr<familyline::logic::GameEntity> attacker, attackee;

    bool _updated = false;

    /**
     * Can add entities and control them?
     *
     * It will be false only when reproducing a recorded game
     */
    bool can_control_ = false;

    familyline::PreviewRenderer* pr_ = nullptr;

    bool front = false, back = false;
    bool left = false, right = false;
    bool rotate_left = false, rotate_right = false;
    bool mouse_click = false;
    bool exit_game   = false;

    bool attack_set = false, attack_ready = false;

    bool remove_object = false;

    bool zoom_in          = false;
    bool zoom_out         = false;
    double zoom_factor    = 0;
    bool zoom_mouse       = false;
    bool build_something  = false;
    bool preview_building = false;
    bool build_tent = false, build_tower = false;

    bool do_something = false;

    const familyline::input::CommandTable& ctable_;

public:
    bool renderBBs = false;

    HumanPlayer(
        familyline::logic::PlayerManager& pm, const familyline::logic::Terrain& t, const char* name,
        uint64_t code, const familyline::input::CommandTable& ctable,  bool can_control_);

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
    void setPreviewer(familyline::PreviewRenderer* pr) { pr_ = pr; }
    void SetPicker(familyline::input::InputPicker* ip);
    void SetInputManager(familyline::input::InputManager*){};

    virtual ~HumanPlayer();
};

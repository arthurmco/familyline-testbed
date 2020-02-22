#pragma once

/**
 * Part of the implementation for the build action
 *
 * An input action is basically a sum type of all the actions you
 * can do in game
 *
 * We have two:
 *  - the human input action, for things that only humans (read: the one
 *    that can click on the menus) can do, like click, move the mouse
 *  - the player input action, for things that all players can do, like
 *    request objects to be build, and position the camera.
 *
 *
 * Copyright (C) 2020 Arthur Mendes
 */

#include <cstdint>
#include <optional>
#include <variant>
#include <SDL2/SDL.h>


/// Simple Events
struct ClickAction {
    int screenX, screenY;
    int buttonCode;
    int clickCount;
    bool isPressed;
};

struct MouseAction {
    int screenX, screenY;
};

struct KeyAction {
    SDL_Keycode keycode;
    const char* keyname;
    bool isPressed, isRepeated;
    uint16_t modifiers;
};

struct WheelAction {
    int screenX, screenY;
    int scrollX, scrollY;
};

struct GameExit {
    int why;
};

////

using HumanInputType = std::variant<ClickAction, MouseAction, KeyAction, WheelAction, GameExit>;

/// Complex events

/**
 * Build some object
 */
struct BuildAction {
    std::string type_name;
};

/**
 * Select an object
 */
struct ObjectSelectAction {
    long int objectID;
};


/**
 * Move the selected object to some location
 */
struct ObjectMoveAction {
    double destX, destZ;
};

/**
 * Make the selected object do the 'use' action in some other object.
 * This action varies from object to object. It can be harvest, attack
 * or trade
 *
 * It is the action you run when you right-click.
 */
struct ObjectUseAction {
    long int useWhat;
};

/**
 * Make the selected object run the specified action
 *
 * The action is a string, it can be anything, but the selected object
 * should understand
 */
struct ObjectRunAction {
    std::string actionName;
};
    
/**
 * Controls camera positioning
 *
 * Might be useful on recorded games, to see and study where the player
 * focus on.
 */
struct CameraMove {
    double deltaX, deltaY;
    double deltaZoom;
};

using PlayerInputType =
    std::variant<BuildAction, ObjectSelectAction, ObjectMoveAction,
                 ObjectUseAction, ObjectRunAction, CameraMove>;

struct HumanInputAction {
    uint64_t timestamp;
    HumanInputType type;
};

struct PlayerInputAction {
    uint64_t timestamp;
    uint64_t playercode;
    uint32_t tick;
    
    PlayerInputType type;
};


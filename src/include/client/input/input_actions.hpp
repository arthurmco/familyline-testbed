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

using HumanInputType = std::variant<ClickAction, MouseAction,
                                    KeyAction, WheelAction, GameExit>;


struct HumanInputAction {
    uint64_t timestamp;
    HumanInputType type;
};


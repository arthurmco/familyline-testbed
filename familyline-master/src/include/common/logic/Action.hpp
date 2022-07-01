/*
    Action information

    Copyright 2016, 2017 Arthur M
*/

#include <cstddef>
#include <cstring>
#include <functional>

#include "game_entity.hpp"

#ifndef ACTION_HPP
#define ACTION_HPP

namespace familyline::logic
{
/* An action is any predefined command done by an object.
   Examples are: train unit, build construction, repair...

   They usually are done by the player, but the commands can be invoked
   from the entity
*/

struct Action;

/* Action event data */
struct ActionData {
    /* Player positional data.
       Mostly used for building objencts */
    float xPos, yPos;

    /* The object that registered the action */
    GameEntity* actionOrigin;
};

/* Represents an action handler
   It's run each time an action is invoked

   An action might be invoked by two ways:
   - in the user interface
   - manually invoking it in the ActionManager

   Returns true if handler has been accepted, false if not (e.g you
   have insufficient money to run the action)
*/
typedef std::function<bool(Action* ac, ActionData acdata)> ActionHandler;

/* Represents an individual action */
struct Action {
    std::string name;
    std::string assetname;
    ActionHandler handler = nullptr;
    size_t refcount       = 1;

    Action(const char* name, const char* assetname) : name(name), assetname(assetname) {}

    Action() : Action("", "") {}
};

}  // namespace familyline::logic

#endif /* end of include guard: ACTION_HPP */

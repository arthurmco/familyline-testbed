/*
  Abstract class that represents a listener for game actions

  Copyright (C) 2017 Arthur M
*/

#ifndef GAMEACTIONLISTENER_HPP
#define GAMEACTIONLISTENER_HPP

#include <string>

#include "GameAction.hpp"

namespace familyline::logic
{
/*
 *  Abstract class that implements a listener for actions
 *
 *  Useful for systems that take actions based on object creation/removal (like
 * networking, who need to send actions to the server
 *
 *  The player classes get the inputs and send to the game action manager, who
 * sends them to the game action listeners.
 *
 */

class GameActionListener
{
protected:
    std::string _name;

public:
    GameActionListener(const char* name) : _name(name) {}

    virtual void OnListen(GameAction&) = 0;

    virtual ~GameActionListener() {}

    const char* GetName() const { return _name.c_str(); }
};

}  // namespace familyline::logic

#endif /* GAMEACTIONLISTENER_HPP */

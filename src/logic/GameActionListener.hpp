/*
  Abstract class that represents a listener for game actions

  Copyright (C) 2017 Arthur M
*/

#ifndef GAMEACTIONLISTENER_HPP
#define GAMEACTIONLISTENER_HPP

#include <string>
#include "GameAction.hpp"

namespace Tribalia::Logic {

/*
 * Abstract class that implements a listener for actions
 *
 * Useful for systems that take actions based on object creation/removal (like
 * networking, who need to send actions to the server
 */

class GameActionListener {
protected:
    std::string _name;
    
public:
    GameActionListener(const char* name) : _name(name) {}
    
    virtual void OnListen(GameAction&) = 0;

    virtual ~GameActionListener() {}

    const char* GetName() const { return _name.c_str(); }
};

}

#endif /* GAMEACTIONLISTENER_HPP */

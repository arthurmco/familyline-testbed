/***
    Manages game actions for players

    Copyright (C) 2017 Arthur M
 ***/

#ifndef GAMEACTIONMANAGER_HPP
#define GAMEACTIONMANAGER_HPP

#include <queue>
#include <list>

#include "GameAction.hpp"
#include "GameActionListener.hpp"

namespace Tribalia::Logic {

/* 
   The game action manager acts like a queue of actions from the game.
   It registers all game actions from one or more players, until a certain
   limit. 

   Useful for saving them to be reproduced later (like a save game), or 
   transmissing them via network
*/


class GameActionManager {
private:
    std::queue<GameAction> _actions;
    std::list<GameActionListener*> _listeners;

    bool Pop(GameAction& a);
    
public:
    void Push(GameAction a);

    void AddListener(GameActionListener*);

    void ProcessListeners();
};

}


#endif /* GAMEACTIONMANAGER_HPP */

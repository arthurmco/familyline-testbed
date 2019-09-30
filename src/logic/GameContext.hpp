/***
    Defines the game context.
    A game context is a structure that pass through all players inside the
    game

    Copyright (C) 2016 Arthur M

***/

#include "object_manager.hpp"

#ifndef GAMECONTEXT_HPP
#define GAMECONTEXT_HPP

namespace familyline::logic {

    struct GameContext {
        ObjectManager* om;
        unsigned long long game_tick;
        
        double elapsed_seconds;
    };


}



#endif /* end of include guard: GAMECONTEXT_HPP */

/***
    Defines the game context.
    A game context is a structure that pass through all players inside the
    game

    Copyright (C) 2016 Arthur M

***/

#include "ObjectManager.hpp"

#ifndef GAMECONTEXT_HPP
#define GAMECONTEXT_HPP

namespace Tribalia {
namespace Logic {

    struct GameContext {
        ObjectManager* om;
        double elapsed_seconds;
    };


}
}



#endif /* end of include guard: GAMECONTEXT_HPP */

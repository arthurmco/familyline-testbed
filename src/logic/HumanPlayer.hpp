/***
    Definitions for human player

    Copyright (C) 2016 Arthur M

***/

#include "Player.hpp"

#ifndef HUMAN_PLAYER
#define HUMAN_PLAYER

namespace Tribalia {
namespace Logic {

    class HumanPlayer : public Player
    {
    public:
        HumanPlayer(const char* name, int elo=0, int xp=0);


        /***
            Virtual function called on each iteration.

            It allows player to decide its movement
            (input for humans, AI decisions for AI... )

            Returns true to continue its loop, false otherwise.
        ***/
        virtual bool Play(void);
    };

}
}



#endif /* end of include guard: HUMAN_PLAYER */

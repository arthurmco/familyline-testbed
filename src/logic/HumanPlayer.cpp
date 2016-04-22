#include "HumanPlayer.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;

HumanPlayer::HumanPlayer(const char* name, int elo, int xp)
    : Player(name, elo, xp)
    {
        /* Initialize input subsystems */

    }


/***
    Virtual function called on each iteration.

    It allows player to decide its movement
    (input for humans, AI decisions for AI... )

    Returns true to continue its loop, false otherwise.
***/

#include <cstdio>

bool HumanPlayer::Play(void){

    /* Receive and process human input */
    printf("> ");
    char s[64];
    fgets(s, 64, stdin);

    //remove '\n'
    s[strlen(s)-1] = 0;

    if (!strcmp(s, "quit")){
        return false;
    }

    return true;

}

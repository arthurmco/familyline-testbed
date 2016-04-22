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

bool HumanPlayer::Play(GameContext* gctx){

    /* Receive and process human input */
    printf("\033[1m> \033[0m");
    char s[64];
    fgets(s, 64, stdin);

    //remove '\n'
    s[strlen(s)-1] = 0;

    if (!strcmp(s, "quit")){
        return false;
    }

    if (!strcmp(s, "xp")){
        printf("%d\n", _xp);
        return true;
    }

    if (!strcmp(s, "addobject")){
        printf("Adding object...\n");
        char oname[128];
        float ox, oy, oz;

        printf("\tName: ");
        fgets(oname, 128, stdin);

        //remove '\n'
        oname[strlen(oname)-1] = 0;

        printf("\tPosition (x y z): ");
        scanf("%f %f %f", &ox, &oy, &oz);

        AttackableObject* ao = new AttackableObject(0, 1, oname,
            ox, oy, oz, 1000, 1.0, 1.0);

        int nid = gctx->om->RegisterObject(ao);
        printf("\t Sucessfully registered %s (%.2f %.2f %.2f) as ID %d\n",
            ao->GetName(), ao->GetX(), ao->GetY(), ao->GetZ(), nid);

        return true;
    }

    return true;

}

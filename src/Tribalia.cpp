
/***
    Tribalia main file

    Copyright 2016 Arthur M.

***/

#include <cstdio>
#include <cstdlib>

#include "Log.hpp"
#include "logic/HumanPlayer.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;


int main(int argc, char const *argv[]) {
    Log::GetLog()->SetFile(stdout);

    ObjectManager* om = new ObjectManager{};

    GameContext gctx;
    gctx.om = om;

    HumanPlayer hp = HumanPlayer{"Arthur"};
    printf("%-30s \t %4d xp\n\n", hp.GetName(), hp.GetXP());
    bool player = false;
    do {
        player = hp.Play(&gctx);
        fflush(stdin);
    } while (player);

    printf("Exited.\n");

    return 0;
}

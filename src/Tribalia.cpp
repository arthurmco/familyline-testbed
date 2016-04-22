
/***
    Tribalia main file

    Copyright 2016 Arthur M.

***/

#include <cstdio>
#include <cstdlib>

#include "Log.hpp"
#include "logic/HumanPlayer.hpp"

#include "EnviroDefs.h"

using namespace Tribalia;
using namespace Tribalia::Logic;

int main(int argc, char const *argv[]) {
    Log::GetLog()->SetFile(stdout);
    Log::GetLog()->Write("Tribalia v%s", VERSION);
    Log::GetLog()->Write("build date %s, commit %s", __DATE__, COMMIT);

    ObjectManager* om = new ObjectManager{};

    GameContext gctx;
    gctx.om = om;

    HumanPlayer hp = HumanPlayer{"Arthur"};
    printf("%-30s \t %4d xp\n\n", hp.GetName(), hp.GetXP());
    bool player = false;
    do {
        player = hp.Play(&gctx);
        om->DoActionAll();
        fflush(stdin);
    } while (player);

    printf("Exited.\n");

    return 0;
}

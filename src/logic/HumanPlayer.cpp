#include "HumanPlayer.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;


class ConcreteObject : public AttackableObject
{
public:
    ConcreteObject(int oid, const char* name,
        float x, float y, float z) :
    AttackableObject(oid, 2, name,
        x, y, z, 1000, 1.0f, 1.5f)
        {

        }

    virtual bool Initialize(){
        this->_tid = 2;
        return true;
    }

    virtual bool DoAction(void)
    {
        printf("Iteration (%d %s) \n", _oid, _name.c_str());
        return true;
    }
};


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

        ConcreteObject* ao = new ConcreteObject(0, oname, ox, oy, oz);

        int nid = gctx->om->RegisterObject(ao);
        printf("\t Sucessfully registered %s (%.2f %.2f %.2f) as ID %d\n",
            ao->GetName(), ao->GetX(), ao->GetY(), ao->GetZ(), nid);
        fflush(stdin);
        return true;
    }

    if (!strcmp(s, "addalot")) {
        printf("\t How much? :");
        int count = 0;
        scanf("%d", &count);

        Log::GetLog()->Write("Adding %d objects", count);
        srand(count * 2);
        for (int i = 0; i < count; i++) {
            char oname[16];
            sprintf(oname, "Object%d", i);

            float ox = rand()*600.0f;
            float oy = (rand() * 1000) / 10.0f;
            float oz = rand() * 1500.0f;

            ConcreteObject* ao = new ConcreteObject(0, oname, ox, oy, oz);
            gctx->om->RegisterObject(ao);
        }
        Log::GetLog()->Write("%d objects added", count);


    }

    if (!strcmp(s, "objcount")){
        printf("%d\n", gctx->om->GetCount());
        return true;
    }

    return true;

}

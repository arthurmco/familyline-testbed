
/***
    Tribalia main file

    Copyright 2016 Arthur M.

***/

#include <cstdio>
#include <cstdlib>

#include "Log.hpp"
#include "logic/AttackableObject.hpp"
#include "logic/ObjectManager.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;

int main(int argc, char const *argv[]) {
    Log::GetLog()->SetFile(stdout);

    AttackableObject* am = new AttackableObject{0x32, 2, "Test", 1.5, 3.5, 2.2,
        1000, 0.75, 3.25};

    printf("Before registration: ");
    printf("\tID: %d\n", am->GetObjectID());
    printf("\tType: %d\n", am->GetTypeID());
    printf("\tPosition: (%.2f, %.2f, %.2f)\n",
        am->GetX(), am->GetY(), am->GetZ());
    printf("\tHP: %.1f\n", am->GetHP());
    printf("\tBase Attack: %.3f\n", am->GetBaseAttack());
    printf("\tBase Armor: %.3f\n", am->GetBaseArmor());

    ObjectManager* objm = new ObjectManager{};
    objm->RegisterObject(am);

    printf("After registation: ");
    printf("\tID: %d\n", am->GetObjectID());
    printf("\tType: %d\n", am->GetTypeID());
    printf("\tPosition: (%.2f, %.2f, %.2f)\n",
        am->GetX(), am->GetY(), am->GetZ());
    printf("\tHP: %.1f\n", am->GetHP());
    printf("\tBase Attack: %.3f\n", am->GetBaseAttack());
    printf("\tBase Armor: %.3f\n", am->GetBaseArmor());

    objm->UnregisterObject(am);

    puts("\n\n");

    return 0;
}

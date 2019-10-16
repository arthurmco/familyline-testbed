/***
    Tent building for Familyline

    Copyright (C) 2016 Arthur M
***/
#include "../logic/game_object.hpp"
#include "ObjectMacros.h"

#ifndef TENT_H
#define TENT_H

namespace familyline {

/*  A tent is a basic people storage building */
class Tent : public logic::GameObject {
private:
    static const int TID = 2;
public:

    Tent();

/*  250 HP, no baseAtk, 1.0 baseArmor, 1.0 building material, 0.95 bulding
    strength, 2 units of garrison capacity */

    /* Called on object initialization */
    virtual bool Initialize();

    /* Called on each engine loop, when an action is performed */
    virtual bool DoAction(void);

    CLONE_MACRO_H(Tent)
//    ADD_CLONE_MACRO(Tent, "Tent")

};

}



#endif /* end of include guard: TENT_H */

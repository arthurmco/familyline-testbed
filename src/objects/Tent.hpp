/***
    Tent building for Tribalia

    Copyright (C) 2016 Arthur M
***/

#include "../logic/Building.hpp"
#include "ObjectMacros.h"

#ifndef TENT_H
#define TENT_H

namespace Tribalia {

/*  A tent is a basic people storage building */
class Tent : public Logic::Building {
private:
    static const int TID = 002;
public:
    Tent(int oid, float x, float y, float z);

    /* Called on object initialization */
    virtual bool Initialize();

    /* Called on each engine loop, when an action is performed */
    virtual bool DoAction(void);
};

}



#endif /* end of include guard: TENT_H */

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
class Tent : Logic::Building {
private:
    static const int TID = 002;
public:
    Tent(int oid, float x, float y, float z);
};

}



#endif /* end of include guard: TENT_H */

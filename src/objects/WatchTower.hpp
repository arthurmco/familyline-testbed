/***
    Tent building for Familyline

    Copyright (C) 2016 Arthur M
***/

#include "../logic/Building.hpp"
#include "ObjectMacros.h"

#ifndef WATCHTOWER_H
#define WATCHTOWER_H

namespace Familyline {

/*  A watch tower is a basic watch building */
class WatchTower : public Logic::Building {
private:
    static const int TID = 3;
public:
	
    WatchTower();

    WatchTower(int oid, float x, float y, float z);

    /* Called on object initialization */
    virtual bool Initialize();

    /* Called on each engine loop, when an action is performed */
    virtual bool DoAction(void);

    CLONE_MACRO_H(WatchTower)
//    ADD_CLONE_MACRO(WatchTower, NULL)

    WatchTower(const WatchTower&);
};

}



#endif /* end of include guard: WATCHTOWER_H */

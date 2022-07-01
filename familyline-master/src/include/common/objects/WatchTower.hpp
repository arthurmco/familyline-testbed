/***
    Tent building for Familyline

    Copyright (C) 2016 Arthur M
***/

#include "../logic/game_entity.hpp"
#include "ObjectMacros.h"

#ifndef WATCHTOWER_H
#define WATCHTOWER_H

namespace familyline
{
// TODO: how to fit this in the new logic system?

/*  A watch tower is a basic watch building */
class WatchTower : public logic::GameEntity
{
private:
    static const int TID = 3;

public:
    WatchTower();

    /* Called on object initialization */
    virtual bool Initialize();

    /* Called on each engine loop, when an action is performed */
    virtual bool DoAction(void);

    CLONE_MACRO_H(WatchTower)
    //    ADD_CLONE_MACRO(WatchTower, NULL)
};

}  // namespace familyline

#endif /* end of include guard: WATCHTOWER_H */

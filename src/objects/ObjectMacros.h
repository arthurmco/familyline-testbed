/***
    Macros for turn your life easier when adding objects

    Copyright (C) 2016 Arthur M
***/

#include "../logic/Building.hpp"
#include "../logic/Unit.hpp"
#include "../graphical/AssetManager.hpp"

#ifndef OBJECTMACROS_H
#define OBJECTMACROS_H

#define DEF_MESH(asset) \
    SetMesh(Tribalia::Graphics::AssetManager::GetInstance()->GetAsset(asset)->  \
        asset.mesh)

/*  Build a game object, generically.
    So, your object always needs to have a constructor like (oid, x, y ,z) */
#define BUILD_GAME_OBJECT_PTR(object, oid, x, y, z) \
    new ##object (oid, x, y, z)

#endif /* end of include guard: OBJECTMACROS_H */

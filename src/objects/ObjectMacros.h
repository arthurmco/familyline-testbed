/***
    Macros for turn your life easier when adding objects

    Copyright (C) 2016 Arthur M
***/

#include "../logic/Building.hpp"
#include "../logic/Unit.hpp"
#include "../graphical/AssetManager.hpp"

#ifndef OBJECTMACROS_H
#define OBJECTMACROS_H

#define DEF_MESH(dasset) SetMesh(new Tribalia::Graphics::Mesh{\
        Tribalia::Graphics::AssetManager::GetInstance()->GetAsset(dasset)->asset.mesh->GetVertexData()})

/*  Build a game object, generically.
    So, your object always needs to have a constructor like (oid, x, y ,z) */
#define BUILD_GAME_OBJECT_PTR(object, oid, x, y, z) \
    new ##object (oid, x, y, z)

#endif /* end of include guard: OBJECTMACROS_H */

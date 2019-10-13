/***
    Macros for turn your life easier when adding objects

    Copyright (C) 2016, 2018 Arthur M
***/
#ifndef OBJECTMACROS_H
#define OBJECTMACROS_H

#include "../logic/game_object.hpp"
#include "../logic/object_components.hpp"
#include "../graphical/asset_manager.hpp"
#include "../graphical/gfx_service.hpp"

#include <memory>
#include "../graphical/mesh.hpp"

#define DEF_MESH(dasset) this->cLocation.value().mesh = \
        std::dynamic_pointer_cast<familyline::graphics::Mesh>(familyline::graphics::GFXService::getAssetManager()->getAsset(dasset))

/*  Build a game object, generically.
    So, your object always needs to have a constructor like (oid, x, y ,z) */
#define BUILD_GAME_OBJECT_PTR(object, oid, x, y, z) \
    new ##object (oid, x, y, z)


#define CLONE_MACRO_H(classname) \
    virtual std::shared_ptr<GameObject> create();


#define CLONE_MACRO_CPP(classname)                              \
    std::shared_ptr<GameObject> classname ::create() {  \
        auto* c = new classname();                              \
        return std::make_shared< classname >();               \
    }

#endif /* end of include guard: OBJECTMACROS_H */

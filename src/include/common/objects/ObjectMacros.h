/***
    Macros for turn your life easier when adding objects

    Copyright (C) 2016, 2018 Arthur M
***/
#ifndef OBJECTMACROS_H
#define OBJECTMACROS_H

#include <common/logic/game_object.hpp>
#include <common/logic/object_components.hpp>
#include <client/graphical/asset_manager.hpp>
#include <client/graphical/gfx_service.hpp>

#include <memory>
#include <client/graphical/mesh.hpp>

// Define PI if it is absent, like in Visual C++
#ifndef M_PI
#define M_PI 3.141592653589
#endif

#define DEF_MESH(dasset) this->cLocation.value().mesh = \
        std::dynamic_pointer_cast<familyline::graphics::Mesh>(familyline::graphics::GFXService::getAssetManager()->getAsset(dasset))

/*  Build a game object, generically.
    So, your object always needs to have a constructor like (oid, x, y ,z) */
#define BUILD_GAME_OBJECT_PTR(object, oid, x, y, z) \
    new ##object (oid, x, y, z)


#define CLONE_MACRO_H(classname)                    \
    virtual std::shared_ptr<GameObject> create();


#define CLONE_MACRO_CPP(classname)                              \
    std::shared_ptr<GameObject> classname ::create() {          \
    auto* c = new classname();                                  \
    c->_id = -1;                                                \
    this->cLocation = std::make_optional<LocationComponent>();  \
    this->cLocation.value().object = this;                      \
    this->cAttack = std::make_optional<AttackComponent>();          \
    this->cAttack.value().object = this;                        \
    return std::shared_ptr< classname >(c);                     \
    }

#endif /* end of include guard: OBJECTMACROS_H */

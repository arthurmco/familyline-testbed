/***
    Material opening generic class

    Copyright (C) 2016 Arthur M.

***/
#pragma once

#include <vector>

#include <client/graphical/gfx_service.hpp>
#include <client/graphical/exceptions.hpp>
#include <client/graphical/material.hpp>

namespace familyline::graphics
{
class MaterialOpener
{
protected:
    Material* _material;

public:
    virtual std::vector<Material*> Open(const char* file) = 0;
};

}  // namespace familyline::graphics

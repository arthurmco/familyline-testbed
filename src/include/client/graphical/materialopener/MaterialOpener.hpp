/***
    Material opening generic class

    Copyright (C) 2016 Arthur M.

***/

#include <vector>

#include "../TextureOpener.hpp"
#include "../exceptions.hpp"
#include "../material.hpp"

#ifndef MATERIALOPENER_HPP
#define MATERIALOPENER_HPP

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

#endif /* end of include guard: MATERIALOPENER_HPP */

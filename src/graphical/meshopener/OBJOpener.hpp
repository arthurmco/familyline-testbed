/***
    .OBJ model opener

    Copyright (C) 2016 Arthur Mendes.

***/

#include <cstdio>
#include <cstring>
#include <cctype>

#include "MeshOpener.hpp"
#include "../MaterialManager.hpp"

#ifndef OBJOPENER_HPP
#define OBJOPENER_HPP

namespace Familyline {
namespace Graphics {

class OBJOpener : public MeshOpener
{
public:
    virtual Mesh* Open(const char* file);
};

}
}

#endif /* end of include guard: OBJOPENER_HPP */

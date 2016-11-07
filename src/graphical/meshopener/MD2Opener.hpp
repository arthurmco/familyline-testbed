/***
    MD2 model opener

    Copyright (C) 2016 Arthur M.

***/

#include <cstdio>
#include <cstring>
#include <cctype>

#include "MeshOpener.hpp"
#include "../MaterialManager.hpp"

#ifndef MD2OPENER_HPP
#define MD2OPENER_HPP

namespace Tribalia {
namespace Graphics {

class MD2Opener : public MeshOpener
{
public:
    virtual Mesh* Open(const char* file);
};

}
}

#endif /* end of include guard: MD2OPENER_HPP */

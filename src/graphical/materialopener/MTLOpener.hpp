/***
    .MTL (Wavefront OBJ material) material opener

    Copyright (C) 2016 Arthur M.

***/
#include <cstring>

#include "MaterialOpener.hpp"
#include "../TextureManager.hpp"

#ifndef MTLOPENER_HPP
#define MTLOPENER_HPP

namespace Tribalia {
namespace Graphics {

    class MTLOpener : public MaterialOpener
    {
    public:
        virtual std::vector<Material*> Open(const char* file);
    };

}
}

#endif /* end of include guard: MTLOPENER_HPP */

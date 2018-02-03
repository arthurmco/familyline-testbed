/***
    Mesh opening generic class

    Copyright (C) 2016 Arthur M.

***/

#include "../Mesh.hpp"
#include "../GFXExceptions.hpp"
#include "Log.hpp"

#ifndef MESHOPENER_HPP
#define MESHOPENER_HPP

namespace Tribalia {
namespace Graphics {

class MeshOpener
{
protected:
    Mesh* _mesh;
public:
    virtual Mesh* Open(const char* file) = 0;
    
};

}
}

#endif /* end of include guard: MESHOPENER_HPP */

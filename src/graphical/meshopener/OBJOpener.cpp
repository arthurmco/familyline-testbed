#include "OBJOpener.hpp"

using namespace Tribalia::Graphics;

Mesh* OBJOpener::Open(const char* file)
{
    FILE* fOBJ = fopen(file, "r");

    if (!fOBJ) {
        throw mesh_exception("Failure to open mesh", errno, file);
    }

    return NULL;

}

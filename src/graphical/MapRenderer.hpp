/***
    Map rendering class

    Copyright (C) 2016 Arthur M

***/
#include "../logic/Map.hpp"

#include "VertexData.hpp"
#include "MaterialManager.hpp"
#include "Material.hpp"

#ifndef MAP_RENDERER_HPP
#define MAP_RENDERER_HPP

namespace Tribalia {
namespace Graphics {

class MapRenderer {
private:
    Tribalia::Logic::Map* _map;

public:
    MapRenderer(Tribalia::Logic::Map* map);

    /*  Size of terrain unit, in OpenGL units.
        Remember: 1 meter = 1 unit */
    static constexpr double UNIT_SIZE = 0.25;

    /* Height of the terrain, in units */
    static constexpr double HEIGHT_SIZE = 0.005;

    /*  Chunk count, in terrain units. */
    static constexpr int CHUNK_SIZE = 128;

    /*  Create terrain vertex data on the variable 'd'.
        Returns true if the chunk could be rendered */
    bool DrawChunk(int x, int y, VertexData* d);

};

}
}



#endif /* end of include guard: MAP_RENDERER_HPP */

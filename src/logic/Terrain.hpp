/***
    Basic terrain representation

    Copyright (C) 2016 Arthur M.

***/

#include <string>
#include <cmath>

#include "../Log.hpp"

#ifndef TERRAIN_HPP
#define TERRAIN_HPP

namespace Tribalia {
namespace Logic {

    /*  A terrain is divided into sections.
        The sections are stored contiguously in the terrain file (and memory)
        They make terrain loading and rendering much more easier
        (because only one portion needs to be loaded/rendered at a time) */

    #define SECTION_SIDE 256
    struct TerrainData {
        struct TerrainSlot {
            int16_t elevation;
            uint16_t terrain_type;
        } data[SECTION_SIDE*SECTION_SIDE];
    };

    class Terrain {
    private:
        int _width, _height;

        std::string _name;
        std::string _description;

        int _section_width, _section_height;
        TerrainData** _data;

    public:
        Terrain(int w, int h);

        TerrainData* GetSection(int index);
        TerrainData* GetSection(int x, int y);
        TerrainData** GetAllSections();

        int GetWidth() const;
        int GetHeight() const;
        int GetSectionCount() const;

        const char* GetName() const;
        const char* GetDescription() const;

    };

}
}



#endif /* end of include guard: TERRAIN_HPP */

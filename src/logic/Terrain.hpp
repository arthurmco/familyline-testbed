/***
    Basic terrain representation

    Copyright (C) 2016, 2018 Arthur Mendes.

***/

#include <string>
#include <cmath>
#include <cstdlib> //memcpy()

#include "Log.hpp"

#ifndef TERRAIN_HPP
#define TERRAIN_HPP

namespace familyline::logic {

    struct TerrainSlot {
	  int16_t elevation;
	  uint16_t terrain_type;
     };

    #define SECTION_SIDE 256
    struct TerrainData {
	TerrainSlot data[SECTION_SIDE*SECTION_SIDE];
    };

    /**
     * \brief Stores the terrain data
     *
     * Here, we divide the terrain is divided into sections.
     *
     * The sections are stored contiguously in the terrain file (and memory).
     * They make terrain loading and rendering much more easier
     * (because only one portion needs to be loaded/rendered at a time)
     */
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

	/**
	 * Gets the height from a point, in game space 
	 */
	int GetHeightFromPoint(unsigned x, unsigned y) const;

        const char* GetName() const;
        const char* GetDescription() const;

	void SetName(const char*);
	void SetDescription(const char*);

	/**
	 * Get raw terrain data and split it into sections 
	 */
	void SetData(TerrainSlot* slot);

    };

}

#endif /* end of include guard: TERRAIN_HPP */

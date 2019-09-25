#pragma once

#include "terrain_renderer.hpp"
#include <vector>

namespace familyline::logic {

    struct TerrainSlot {
        int y;
        int type;
    };

    class Terrain {
        friend class TerrainRenderer;

    private:
        std::vector<TerrainSlot> _slots;
        int _xsize, _zsize;
        TerrainRenderer* _renderer;

    public:
        Terrain(int xsize, int zsize, const std::vector<TerrainSlot>& slots);


        glm::vec2 getSize() const;

        /**
         * Get the height, in game space, from a certain point
         */
        float getHeightFromPoint(float x, float z) const;

        /**
         * Get the inclination, in game space, between two points
         *
         * Return the inclination angle in radians
         */
        float getInclinationFromPoint(glm::vec2 s, glm::vec2 e) const;

        const TerrainRenderer& getRenderer() const;

	
    };

}

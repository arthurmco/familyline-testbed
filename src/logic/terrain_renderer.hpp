#pragma once

class Camera;
class Terrain;

#include <glm/glm.hpp>

namespace familyline::logic {
/**
 * Controls terrain rendering
 */
    class TerrainRenderer {

    private:
        Terrain& _terrain;

        bool _needsUpdate;

    public:

        TerrainRenderer(Terrain& t);


        /**
         * Update the terrain: if needed, send the data from the current quadrant and
         * its neighbors to the video card.
         * But does not render yet
         */
        void update(const Camera& c);


        /**
         * Render the current area of the terrain
         */
        void render(const Camera& c);

        /**
         * Convert a coordinate from game space to model space 
         */
        glm::vec3 convertToModelSpace(glm::vec3 terrainPos) const;


        ~TerrainRenderer();
    };
}

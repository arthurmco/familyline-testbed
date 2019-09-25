#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace familyline::logic {

/**
 * A debug drawer interface, so we can draw some lines and paths from the logic
 * system without being acoplated to it
 */
    class DebugDrawer {
    private:

    public:
        virtual void drawLine(glm::vec2 start, glm::vec2 end, glm::vec4 color) = 0;
        virtual void drawSquare(glm::vec2 start, glm::vec2 end, glm::vec4 foreground, glm::vec4 background) = 0;
        virtual void drawCircle(glm::vec2 point, glm::vec2 radius, glm::vec4 foreground, glm::vec4 background) = 0;
	
        void drawPath(std::vector<glm::vec2> points, glm::vec4 color);

    };

}

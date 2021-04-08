/***
 * Debug draw functions
 *
 * (C) 2019 Arthur Mendes
 */

#pragma once

#include <common/logic/terrain.hpp>
#include <glm/glm.hpp>
#include <vector>

#include <algorithm>
#include <iterator>


namespace familyline::logic
{
/**
 * A debug drawer interface, so we can draw some lines and paths from the logic
 * system without being acoplated to it
 *
 * Will be useful to show the user values from the logic engine directly where
 * it matters (for exemple, rotation or size of an object in a form of cube),
 * instead of showing a lot of numbers in the screen
 */
class DebugDrawer
{
protected:
    const Terrain& terr_;

public:
    virtual void drawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color) = 0;
    virtual void drawSquare(
        glm::vec3 start, glm::vec3 end, glm::vec4 foreground, glm::vec4 background) = 0;
    virtual void drawCircle(
        glm::vec3 point, glm::vec3 radius, glm::vec4 foreground, glm::vec4 background) = 0;

    void drawPath(std::vector<glm::vec3> points, glm::vec4 color);

    template <typename Container>
    void drawPath(Container points, glm::vec4 color)
    {
        std::vector<glm::vec3> nv;
        nv.reserve(points.size());

        std::transform(points.begin(), points.end(), std::back_inserter(nv), [](glm::vec2 v) {
            return glm::vec3(v.x, 5, v.y);
        });

        drawPath(nv, color);
    }

    /// Update some internal structure
    virtual void update() = 0;

    DebugDrawer(const Terrain& terr) : terr_(terr) {}

    virtual ~DebugDrawer() {}
};

}  // namespace familyline::logic

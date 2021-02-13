/***
 * Debug draw functions
 *
 * (C) 2019 Arthur Mendes
 */

#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <common/logic/terrain.hpp>


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

    /// Update some internal structure
    virtual void update() = 0;

    DebugDrawer(const Terrain& terr)
        : terr_(terr)
        {}

    virtual ~DebugDrawer() {}
};

}  // namespace familyline::logic

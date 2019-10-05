#include "debug_drawer.hpp"

using namespace familyline::logic;


void DebugDrawer::drawPath(std::vector<glm::vec2> points, glm::vec4 color)
{
	for (unsigned i = 1; i < points.size(); i++) {
		this->drawLine(points[i - 1], points[i], color);
	}
}
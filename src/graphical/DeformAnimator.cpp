#include "DeformAnimator.hpp"

using namespace Tribalia::Graphics;

void DeformAnimator::AddFrame(unsigned frameno, std::vector<glm::vec3> vertices)
{
	this->_frames[frameno] = vertices;
}

const std::vector<glm::vec3>& DeformAnimator::getVertices(unsigned frameno)
{
	return this->_frames[frameno];
}

const size_t DeformAnimator::getFrameCount() const
{
	return this->_framecount;
}
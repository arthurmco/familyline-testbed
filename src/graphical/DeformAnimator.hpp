/**
  * Animator based on mesh deformation, the simplest way of animating something
  * It receives a frame number and a list of vertices.
  * When we animate, it gets the list of vertices directly or interpolate between two
  * lists, depending on the framerate of the animation.
  *
  * Copyright (C) 2018 Arthur M
  */

#ifndef DEFORMANIMATOR_HPP
#define DEFORMANIMATOR_HPP

#include <map>
#include <vector>
#include "VertexData.hpp"

namespace Familyline::Graphics {

	class DeformAnimator : public BaseAnimator {
	
	private:
		unsigned _framecount;
		
		std::map<unsigned /*frame*/, std::vector<glm::vec3>> _frames;

	public:
		const unsigned DEFAULT_FPS = 60;

		DeformAnimator(VertexData* vd, unsigned framecount)
			: BaseAnimator(vd), _framecount(framecount)
		{}

		void AddFrame(unsigned frameno, std::vector<glm::vec3> vertices);

		virtual const std::vector<glm::vec3>& getVertices(unsigned frameno);
		virtual size_t getFrameCount() const;
	};

}

#endif // !DEFORMANIMATOR_HPP

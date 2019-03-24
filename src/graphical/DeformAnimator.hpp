/***
 * Animator for mesh deformation animation
 *
 * Copyright (C) 2018 Arthur Mendes
 *
 ***/

#ifndef DEFORMANIMATOR_HPP
#define DEFORMANIMATOR_HPP

#include <map>
#include <string>

#include "Animator.hpp"

namespace familyline::graphics {
    
    /**
     * Mesh deformation animation class
     * 
     * Usually contains instructions for animating
     * vertices one by one
     */
    class DeformAnimator final : public Animator {
    private:
	std::map<std::string /*animation-name*/, std::vector<VertexDataGroup>> _animation_frames;
	int _framerate;

	std::string _animation_name = "default";

	/**
	 * What position in the animation we are.
	 * It's double, because we can be in the middle of two frames
	 */
	double _frameptr = 0.0;

    public:
	DeformAnimator(std::map<std::string /*animation-name*/, std::vector<VertexDataGroup>>,
		       int framerate);

	/** 
	 * Get vertex data from the current frame 
	 */
	virtual VertexDataGroup getCurrentFrame();

	/**
	 * Advance animation by 'ms' milisseconds.
	 * If ms is not the frametime of this animation, interpolate
	 */
	virtual void advance(int ms);

	/**
	 * Run some predefined animation named 'name'.
	 * Set the internal pointer to the first frame of that animation
	 */
	virtual void runAnimation(const char* name);

	
	/**
	 * Clone an animator, returning a new one, with the same
	 * frame data and class type as the old one, but in the
	 * default position
	 */
	virtual Animator* clone() {
	    auto d = new DeformAnimator(
		this->_animation_frames, this->_framerate
		);
	    d->_frameptr = 0.0;

	    return d;
	}
    };

}

#endif

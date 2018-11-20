/***
 * Stub animation class for static models
 *
 * Copyright (C) 2018 Arthur Mendes
 *
 ***/

#ifndef STATICANIMATOR_HPP
#define STATICANIMATOR_HPP

#include "Animator.hpp"

namespace familyline::graphics {

    /**
     * \brief Stub animation class for static models
     *
     * This class is a stub class that fakes an animated model
     * out of a static one, so I only need to have one code 
     * path to return a model
     */
    class StaticAnimator final : Animator {
    private:
	VertexDataGroup _vdg;

    public:
	StaticAnimator(VertexDataGroup vdg);

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
    };

}  // familyline::graphics

#endif /* STATICANIMATOR_HPP */

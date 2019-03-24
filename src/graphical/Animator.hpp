/***
 * Base class to support animation in meshes
 *
 * Copyright (C) 2018 Arthur Mendes
 *
 ***/

#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include "VertexData.hpp"
#include <vector>

namespace familyline::graphics {

    typedef std::vector<struct VertexData> VertexDataGroup;
    
    
    /**
     * \brief Base animator class
     *
     * This animation class is a wrapper to support animation in all meshes
     * \see StaticAnimator
     * \see DeformAnimator
     */
    class Animator {
    public:

	/// Global list of animator objects
	/// We store here so we can run every one of them at once, in the same
	/// thread and place.
	static std::vector<Animator*> animators;
	
    protected:
	/**
	 * The base vertex data, the one that the mesh file
	 * shows in its vertex list
	 */
	VertexDataGroup base_vdata;

    public:
	Animator();
	virtual ~Animator();
	    
	
	/**
	 * Get vertex data from the current frame 
	 */
	virtual VertexDataGroup getCurrentFrame() = 0;

	/**
	 * Advance animation by 'ms' milisseconds
	 *
	 * If ms is not the frametime of this animation, interpolate
	 */
	virtual void advance(int ms) = 0;

	/** 
	 * Run some predefined animation named 'name'
	 *
	 * Set the internal pointer to the first frame of that animation
	 */
	virtual void runAnimation(const char* name) = 0;

	/**
	 * Clone an animator, returning a new one, with the same
	 * frame data and class type as the old one, but in the
	 * default position
	 */
	virtual Animator* clone() = 0;

	static void runAllAnimations(int ms);
    };

    

}  // familyline::graphics


#endif /* ANIMATOR_HPP */

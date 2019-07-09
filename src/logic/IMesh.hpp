/***
    Abstract mesh for the logic part of the game
    
    Copyright 2017 Arthur M
 ***/

#ifndef IMESH_HPP
#define IMESH_HPP

#include <glm/glm.hpp>

namespace familyline::logic {

    /**
     * A logic representation of a mesh, with only the sufficient for the
     * abstract part of the game (the files in the 'logic' folder) to work
     * properly
     *
     * It abstracts away the idea of a mesh, allowing the graphical part
     * to use anything as a mesh
     *
     * \see graphics::Mesh for the concrete mesh implementation of this engine.
     */
    class IMesh {
    public:
	IMesh() {}
	virtual void setLogicPosition(glm::vec3) = 0;

	virtual ~IMesh() {}

    };
    

}  // familyline::logic



#endif /* IMESH_HPP */

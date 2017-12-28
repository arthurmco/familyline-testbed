/***
    Abstract mesh for the logic part of the game
    
    Copyright 2017 Arthur M
 ***/

#ifndef IMESH_HPP
#define IMESH_HPP


namespace Tribalia::Logic {

    /* A logic representation of a mesh, with only the sufficient for the
       abstract part of the game (the files in the 'logic' folder) to work
       properly
    */
    class IMesh {
    public:
	IMesh() {}
	virtual void SetPosition(glm::vec3) = 0;

	virtual ~IMesh() {}

    };
    

}  // Tribalia::Logic



#endif /* IMESH_HPP */

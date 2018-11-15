
/***
    Building base class

    Copyright 2016, 2018 Arthur Mendes.

***/

#ifndef BUILDING_H
#define BUILDING_H

#include "AttackableObject.hpp"
//#include "Unit.hpp"
#include <list>

namespace familyline::logic {

    class Building : public AttackableObject {
    public:
	Building(object_id_t id, const char* type, const char* name,
		 int maxLifePoints, int currLifePoints,
		 AttackAttributes atkAttributes)
	    : AttackableObject(id, type, name, maxLifePoints, currLifePoints,
			       atkAttributes)
	    {}


	/* Clone the object at a specified position
	 * 
	 * This function needs to be implemented for each object, because it
	 * will return the same class as the object
	 *
	 * This is used in the ObjectFactory, to clone the object
	 *
	 * Returns a new object
	 */
	virtual GameObject* clone(glm::vec3 position);


    };
}


#endif /* end of include guard: BUILDING_H */


/***
    Building base class

    Copyright 2016, 2018 Arthur Mendes.

***/

#ifndef BUILDING_H
#define BUILDING_H

#include "AttackableObject.hpp"
//#include "Unit.hpp"
#include <list>

namespace Familyline::Logic {

    class Building : public AttackableObject {
    public:
	Building(object_id_t id, const char* type, const char* name,
		 int maxLifePoints, int currLifePoints,
		 AttackAttributes atkAttributes)
	    : AttackableObject(id, type, name, maxLifePoints, currLifePoints,
			       atkAttributes)
	    {}



    };
}


#endif /* end of include guard: BUILDING_H */

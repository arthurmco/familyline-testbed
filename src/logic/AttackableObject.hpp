
/***
    Attackable object class

    Copyright 2016-2018 Arthur Mendes.

***/

#include "GameObject.hpp"

#include <cstring>
#include <cstdlib> //srand(), rand()

#ifndef ATTACKABLEOBJECT_HPP
#define ATTACKABLEOBJECT_HPP

namespace Familyline::Logic {

    /*
     * Some packed attributes for the attack
     */
    struct AttackAttributes {
	double atkPoints;

	AttackAttributes()
	    : atkPoints(0.0)
	    {}

	AttackAttributes(double atkPoints)
	    : atkPoints(atkPoints)
	    {}
    };

    /**
     * Object that has life points and, therefor, can be attacked.
     * It will not necessarily attack, but it can die
     *
     */
    class AttackableObject : public GameObject {
    private:
	/**
	 * Current and maximum life points
	 */
	int currLifePoints;
	int maxLifePoints;


	/**
	 * Get attack (and defense) attributes
	 */
	AttackAttributes atkAttributes;

    public:

	int getMaxLifePoints() const { return this->maxLifePoints; }
	int getCurrentLifePoints() const { return this->currLifePoints; }
	AttackAttributes getAttackAttributes() const { return this->atkAttributes; }

	AttackableObject(object_id_t id, const char* type, const char* name,
			 int maxLifePoints, int currLifePoints,
			 AttackAttributes atkAttributes);

    };


}


#endif /* end of include guard: ATTACKABLEOBJECT_HPP */

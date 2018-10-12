
/***
    Attackable object class

    Copyright 2016-2018 Arthur Mendes.

***/

#include "GameObject.hpp"

#include <optional>
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
	double armorPoints;

	/// Attack arc, in radians
	/// AoE attacks have 2*MATH_PI radius, a circle.
	/// Direct attacks will have a lower arc
	double atkArc;

	/// Attack range
	/// Simply the radius of said arc above
	double atkRange;

	/// Minor attack 
	/// Attack points in the external areas of the circle
	double atkLower;

	AttackAttributes()
	    : atkPoints(0.0), armorPoints(0.0), atkArc(0.0),
	      atkRange(0.0), atkLower(0.0)
	    {}

	AttackAttributes(double atkPoints, double armorPoints,
			 double atkArc, double atkRange,
			 double atkLower)
	    : atkPoints(atkPoints), armorPoints(armorPoints),
	      atkArc(atkArc), atkRange(atkRange), atkLower(atkLower)
	    {}
    };

    /**
     * Object that has life points and, therefor, can be attacked.
     * It will not necessarily attack, but it can die
     *
     */
    class AttackableObject : public GameObject {
    protected:
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

	/*
	 * Deal damage to this attackable object
	 *
	 * Return the health points after the damage
	 */
	double doDamage(double dmg);
	
	AttackableObject(object_id_t id, const char* type, const char* name,
			 int maxLifePoints, int currLifePoints,
			 AttackAttributes atkAttributes);

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


	/*
	 * Attack other object
	 *
	 * Return the damage caused wrapped around a std::optional.
	 * If the value doesn't exist, the attacked is out of range.
	 * If the value exists, the damage is inside the optional
	 *
	 * (Since I'll change the engine to fixed timesteps, the attacks will be on a fixed rate)
	 */
	std::optional<double> doAttack(AttackableObject* attacked);
    };


}


#endif /* end of include guard: ATTACKABLEOBJECT_HPP */

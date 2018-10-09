#include "AttackableObject.hpp"

#include <cmath> // used in doAttack() function
#include <algorithm> // std::min

using namespace Familyline::Logic;

AttackableObject::AttackableObject(object_id_t id, const char* type, const char* name,
				   int maxLifePoints, int currLifePoints,
				   AttackAttributes atkAttributes)
    : GameObject(id, type, name)
{
    this->maxLifePoints = maxLifePoints;
    this->currLifePoints = currLifePoints;
    this->atkAttributes = atkAttributes;
}

/* Clone the object at a specified position
 *
 * This function needs to be implemented for each object, because it
 * will return the same class as the object
 *
 * This is used in the ObjectFactory, to clone the object
 *
 * Returns a new object
 */
GameObject* AttackableObject::clone(glm::vec3 position)
{
    auto a = new AttackableObject(this->id, this->type, this->name,
				  this->maxLifePoints, this->maxLifePoints,
				  this->atkAttributes);
    a->position = position;
    return a;
}


/*
 * Attack other object
 *
 * Return the damage caused
 * Since the attack area of the component will be always an arc (a sufficiently small
 * one for the straight line attacks, and a 360deg arc for AoE attackers), we'll convert
 * the atk and def coordinates to polar coordinates and do the range calculations there
 *
 * (Since I'll change the engine to fixed timesteps, the attacks will be on a fixed rate)
 */
double AttackableObject::doAttack(AttackableObject* defender)
{
    const double distX = defender->position.x - this->position.x;
    const double distY = defender->position.z - this->position.z;

    const double defDistance = sqrt((distX*distX) + (distY*distY));

    // convert the calculations to attacker
    // it's easier to do the calculations if we assume the attacker angle
    // is always 0deg
    const double defAngle = atan2(distY, distX) - this->rotation;

    const double arcUpper = this->atkAttributes.atkArc/2;
//    const double arcLower = -this->atkAttributes.atkArc/2;

    const double sin_defAngle = sin(defAngle);
    const double cos_defAngle = cos(defAngle);

    if (sin(arcUpper) > sin_defAngle &&
	cos(arcUpper) < cos_defAngle &&
	this->atkAttributes.atkRange > defDistance) {

	const double factor = (1 - std::abs(sin_defAngle/arcUpper));

	// TODO: Occasionally, the armor points will not be considered
	// (it would be unfair with weaker units)

	const double aRange = this->atkAttributes.atkPoints - this->atkAttributes.atkLower;

	const double damage = (this->atkAttributes.atkLower + aRange*factor)
	    - defender->atkAttributes.armorPoints;
	printf("<%.2f>", damage);
	
	return std::max(0.0, damage);
    }

    return 0.0;
}


/*
 * Deal damage to this attackable object
 *
 * Return the health points after the damage
 */
double AttackableObject::doDamage(double dmg)
{
    auto currHP = this->currLifePoints - dmg;
    this->currLifePoints = currHP;
    
    return double(this->currLifePoints);
}

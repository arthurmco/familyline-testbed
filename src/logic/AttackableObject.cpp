#include "AttackableObject.hpp"

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

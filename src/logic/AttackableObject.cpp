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

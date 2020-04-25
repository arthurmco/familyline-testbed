#include <common/logic/game_object.hpp>
#include <common/logic/object_components.hpp>

#include <common/logic/Terrain.hpp>
#include <client/graphical/TerrainRenderer.hpp>

#include <cmath>
#include <algorithm> //for std::max

using namespace familyline::logic;

/**
 * Updates a mesh by getting a coordinate in the logic space, converting it to
 * the graphical/OpenGL space and then setting it as the mesh position
 */
void LocationComponent::updateMesh(const Terrain& t) {
    glm::vec3 pos = this->object->getPosition();

	auto height = t.GetHeightFromPoint(pos.x, pos.z);

    //	this->mesh->setPosition(t.renderer->convertToModelSpace(glm::vec3(pos.x, height, pos.y)));
	this->mesh->setLogicPosition(
        graphics::GameToGraphicalSpace(glm::vec3(pos.x, height, pos.z)));
}


/**
 * Check if the attacked component is in range of the attacker
 */
bool AttackComponent::isInAttackRange(const AttackComponent& other)
{
    return (other.object != nullptr && this->doDirectAttack(other).has_value());

}

/**
 * Attack directly another object
 *
 * Do a certain amount of damage to him.
 * Return the pure damage dealt, or an empty optional if the target is out of range
 */
std::optional<double> AttackComponent::doDirectAttack(const AttackComponent& defender)
{
	if (!this->object->getLocationComponent())
		return std::make_optional(0.0);

	if (!this->object->getLocationComponent())
		return std::make_optional(0.0);

	const auto atkpos = this->object->getPosition();
	const auto defpos = defender.object->getPosition();

	const double distX = defpos.x - atkpos.x;
	const double distY = defpos.z - atkpos.z;

	const double defDistance = sqrt((distX*distX) + (distY*distY));

	// convert the calculations to attacker
	// it's easier to do the calculations if we assume the attacker angle
	// is always 0deg
	const double defAngle = atan2(distY, distX) - this->rotation;

	const double arcUpper = this->range / 2;
	//    const double arcLower = -this->atkAttributes.atkArc/2;

	const double sin_defAngle = std::abs(sin(defAngle));
	const double cos_defAngle = std::abs(cos(defAngle));

    bool all360 = (arcUpper >= M_PI);
    bool validAngle = all360 || (sin(arcUpper) > sin_defAngle &&
                                 cos(arcUpper) < cos_defAngle);
    
	if (validAngle && this->atkDistance > defDistance) {

        //printf("sin (atk/def): %.2f %.2f\t", sin(arcUpper), sin_defAngle);
        //printf("cos (atk/def): %.2f %.2f\t", cos(arcUpper), cos_defAngle);
        //printf("distance (atk/def): %.2f %.2f\n", this->atkDistance, defDistance);

		const double factor = (1 - std::abs(sin_defAngle / arcUpper));

		// TODO: Occasionally, the armor points will not be considered
		// (it would be unfair with weaker units)

		const double aRange = this->atkRanged;

		const double damage = (this->atkRanged + aRange * factor)
			- defender.armor;
		return std::make_optional(std::max(0.0, damage));
	}
    
	return std::nullopt;

}

ColonyComponent::ColonyComponent() 
    : owner(std::optional<std::reference_wrapper<Colony>>())
{}

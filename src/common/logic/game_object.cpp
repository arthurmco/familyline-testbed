#include <algorithm>
#include <common/logic/game_object.hpp>

using namespace familyline::logic;

GameObject::GameObject(
    std::string type, std::string name, glm::vec2 size, int health, int maxHealth, bool showHealth)
    : _type(type),
      _name(name),
      _size(size),
      _health(health),
      _maxHealth(maxHealth),
      _showHealth(showHealth)
{
    this->cLocation                = std::make_optional<LocationComponent>();
    this->cLocation.value().object = this;

    this->cAttack                = std::make_optional<AttackComponent>();
    this->cAttack->range         = 3.14;
    this->cAttack.value().object = this;
}

/**
 * Function to be called by the ObjectFactory, so it can
 * create a game object without knowing its class name,
 * considering you registered it there
 *
 * \see ObjectFactory
 */
std::shared_ptr<GameObject> GameObject::create()
{
    auto cloned = std::make_shared<GameObject>(
        this->_name, this->_type, this->_size, this->_health, this->_maxHealth, this->_showHealth);

    // TODO: copy the components.
    cloned->cAttack = this->cAttack ? std::make_optional<AttackComponent>(this->cAttack.value())
                                    : std::optional<AttackComponent>();
    cloned->cContainer = this->cContainer
                             ? std::make_optional<ContainerComponent>(this->cContainer.value())
                             : std::optional<ContainerComponent>();
    cloned->cLocation = this->cLocation
                            ? std::make_optional<LocationComponent>(this->cLocation.value())
                            : std::optional<LocationComponent>();
    cloned->cMovement = this->cMovement
                            ? std::make_optional<MovementComponent>(this->cMovement.value())
                            : std::optional<MovementComponent>();

    if (cloned->cAttack) cloned->cAttack->object = this;

    if (cloned->cLocation) cloned->cLocation->object = this;

    return cloned;
}

/**
 * Update internal object logic
 */
void GameObject::update() {}

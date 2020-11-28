#pragma once

#include <common/logic/object_components.hpp>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <common/logic/types.hpp>

namespace familyline::logic
{

/**
 * Game object type
 *
 * We can use the absence or presence of certain components to guess type,
 * but is easier to the human and to the computer if we could explicitly say
 * this.
 *
 * The human would not have to guess and think much, and the computer would
 * have to compare only one value. It would also remove ambiguity
 */
enum class ObjectCategory { CommonUnit, Building, ResourceDepot, SiegeUnit, NavalUnit };

/**
 * Our beloved base game object
 */
class GameObject
{
    friend class ObjectManager;

private:
protected:
    std::string _type;
    std::string _name;

    object_id_t _id = -1;

    /**
     * Object hitbox width and length, in game units
     * The X position is the width, the Y position is the length
     *
     * The hitbox center is at the object location
     */
    glm::vec2 _size;

    /**
     * Current and maximum health
     */
    double _health;
    int _maxHealth;

    /**
     * Show health on interface. Might be useful, for example, for trees,
     * or decoration items
     */
    bool _showHealth;

    /**
     * Logic engine position
     *
     * X and Z coordinates are mapped to the X and Y axis on the terrain.
     * Y coordinate is the "height".
     */
    glm::vec3 _position;

protected:
    std::optional<LocationComponent> cLocation;
    std::optional<AttackComponent> cAttack;
    std::optional<ContainerComponent> cContainer;
    std::optional<MovementComponent> cMovement;
    std::optional<ColonyComponent> cColony;

    ObjectCategory category;

public:
    object_id_t getID() const { return _id; }
    const std::string& getType() const { return _type; }
    const std::string& getName() const { return _name; }

    double getHealth() const { return _health; }
    int getMaxHealth() const { return _maxHealth; }
    bool isShowingHealth() const { return _showHealth; }

    double addHealth(double v)
    {
        _health += v;
        return _health;
    }

    glm::vec2 getSize() const { return _size; }

    glm::vec3 getPosition() const { return _position; }
    glm::vec3 setPosition(glm::vec3 v)
    {
        _position = v;
        return _position;
    }

    GameObject(
        std::string type, std::string name, glm::vec2 size, int health, int maxHealth,
        bool showHealth = true);

    /**
     * Function to be called by the ObjectFactory, so it can
     * create a game object without knowing its class name,
     * considering you registered it
     *
     * \see ObjectFactory
     */
    virtual std::shared_ptr<GameObject> create();

    /**
     * Update internal object logic
     */
    virtual void update();

    std::optional<LocationComponent>& getLocationComponent() { return cLocation; }
    std::optional<AttackComponent>& getAttackComponent() { return cAttack; }
    std::optional<ContainerComponent>& getContainerComponent() { return cContainer; }
    std::optional<MovementComponent>& getMovementComponent() { return cMovement; }
    std::optional<ColonyComponent>& getColonyComponent() { return cColony; }

    ObjectCategory getCategory() { return this->category; }

    /**
     * Check if an object supports running a certain command
     *
     * A command is simply something that the object can run.
     * It might be graphically represented by a button into the game.
     */
    virtual bool hasCommand(std::string_view command) { return false; }

    /**
     * Run the specified command, with the specified params
     */
    virtual void runCommand(std::string_view command, std::array<unsigned long long, 5> params) {}
    
    virtual ~GameObject() {}
};
}  // namespace familyline::logic

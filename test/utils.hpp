/**
 * Utilitary functions for easing test object creation
 *
 * Copyright (C) 2020 Arthur Mendes
 */

#include <memory>
#include <functional>

#include <common/logic/game_object.hpp>
#include <common/objects/ObjectMacros.h>

struct object_init {
    std::string type;
    std::string name;
    glm::vec2 size;
    int health;
    int maxHealth;
    bool showHealth;

    std::function<void()> fnUpdate;

    std::optional<familyline::logic::AttackComponent> atkComponent;
};

class TestObject : public familyline::logic::GameObject {
private:
    std::function<void()> fnUpdate_;

    // Preserve the original init parameters to ease object cloning
    struct object_init init_params_;

public:
    TestObject(const struct object_init& init);

    virtual void update();

    CLONE_MACRO_H(TestObject)

};

std::shared_ptr<TestObject> make_object(const struct object_init& init);

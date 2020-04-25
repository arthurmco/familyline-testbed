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
  TestObject(const struct object_init &init);

  virtual void update();

  CLONE_MACRO_H(TestObject)

};

/// Create an object that can have a colony
class TestOwnableObject : public familyline::logic::GameObject {
private:
    std::function<void()> fnUpdate_;

    // Preserve the original init parameters to ease object cloning
    struct object_init init_params_;

public:
    TestOwnableObject(const struct object_init& init);

    virtual void update();

    CLONE_MACRO_H(TestObject)
};

std::shared_ptr<TestObject> make_object(const struct object_init& init);
std::shared_ptr<TestOwnableObject> make_ownable_object(const struct object_init& init);

#include <common/logic/player_manager.hpp>
#include <common/logic/player.hpp>


/// A basic player that mocks a real player.
///
/// It has a custom callback where the player can push actions.
class DummyPlayer : public familyline::logic::Player {
private:    
    std::function<std::vector<familyline::logic::PlayerInputType>(void)> player_input_cb_;
    
public:
    DummyPlayer(familyline::logic::PlayerManager& pm, const char* name, int code,
                decltype(player_input_cb_) input_cb)
        : familyline::logic::Player(pm, name, code), player_input_cb_(input_cb)
        {}

    virtual void generateInput();
};

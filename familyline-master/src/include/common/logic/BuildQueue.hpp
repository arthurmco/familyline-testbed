/*
  The build queue registers items to-be built by the user.

  When you click a building, the build queue schedules its build so it can be
  built in the mouse cursor when you 'click' it again (in this case, calling
  the BuildNext() method)

  Copyright 2017 Arthur M
*/

#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <queue>

#include "Action.hpp"
#include "game_entity.hpp"

#ifndef BUILDQUEUE_HPP
#define BUILDQUEUE_HPP

// TODO: move this to the player
namespace familyline::logic
{
class BuildQueue
{
private:
    std::queue<std::shared_ptr<GameEntity>> _objects;

public:
    void Add(std::shared_ptr<GameEntity>);

    std::shared_ptr<GameEntity> BuildNext(glm::vec3 pos);
    std::optional<std::shared_ptr<GameEntity>> getNext() const;

    void Clear();

    static BuildQueue* GetInstance()
    {
        static BuildQueue* i = nullptr;
        if (!i) i = new BuildQueue();

        return i;
    }
};

bool DefaultBuildHandler(Action* ac, ActionData data, GameEntity* built);
}  // namespace familyline::logic

#endif  // BUILDQUEUE_HPP

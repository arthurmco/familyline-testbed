/*
  The build queue registers items to-be built by the user.

  When you click a building, the build queue schedules its build so it can be
  built in the mouse cursor when you 'click' it again (in this case, calling
  the BuildNext() method)

  Copyright 2017 Arthur M
*/

#include "Action.hpp"
#include "game_object.hpp"
#include <queue>
#include <glm/glm.hpp>
#include <memory>

#ifndef BUILDQUEUE_HPP
#define BUILDQUEUE_HPP

namespace familyline::logic {
    class BuildQueue {
    private:
        std::queue<std::shared_ptr<GameObject>> _objects;

    public:
        void Add(std::shared_ptr<GameObject>);

        std::shared_ptr<GameObject> BuildNext(glm::vec3 pos);
        std::shared_ptr<GameObject> GetNext();
        void Clear();

        static BuildQueue* GetInstance() {
            static BuildQueue* i = nullptr;
            if (!i) i = new BuildQueue();

            return i;
        }
    };

    bool DefaultBuildHandler(Action* ac, ActionData data, GameObject* built);
}


#endif //BUILDQUEUE_HPP

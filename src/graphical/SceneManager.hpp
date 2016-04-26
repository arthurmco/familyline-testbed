/***
    Graphical scene management

    Copyright (C) 2016 Arthur M.

***/

#include <vector>
#include <list>
#include <cstring>

#include "SceneObject.hpp"
#include "Camera.hpp"

#include "../Log.hpp"

#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

namespace Tribalia {
namespace Graphics {

    class SceneManager {
    private:
        std::vector<SceneObject*> _objects;
        std::list<SceneObject*> _valid_objects;
        Camera* _cam;

    public:
        int AddObject(SceneObject*);

        SceneObject* GetObject(const char* name) const;
        SceneObject* GetObject(float x, float y, float z) const;

        void RemoveObject(SceneObject*);

        Camera* GetCamera() const;
        void SetCamera(Camera*);

        /* Update the valid objects list.
            This list is responsable to show the valid objects, the objects
            visible to the camera and some beyond */
        void UpdateValidObjects();

        /* Retrieve the valid objects list */
        std::list<SceneObject*>* GetValidObjects() const;
    };

}
}

#endif /* end of include guard: SCENEMANAGER_HPP */

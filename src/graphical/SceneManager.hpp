/***
    Graphical scene management

    Copyright (C) 2016 Arthur Mendes.

***/

#include <vector>
#include <list>
#include <cstring>

#include "SceneObject.hpp"
#include "Camera.hpp"

#include "Log.hpp"

#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

namespace Familyline {
namespace Graphics {

    /* Scene quadrant size, in OpenGL coordinates */
    #define SCENE_QUADRANT_SIZE 64

    /* A scene quadrant. Might help divide the scene in pieces, so rendering is faster */
    struct SceneQuadrant {
        int x, y;
        std::list<SceneObject*> _quadrant_objects;
    };

    class SceneManager {
    private:
        bool _listModified = true;
        std::vector<SceneObject*> _objects;
        std::list<SceneObject*> _valid_objects;

        SceneQuadrant* _quadrants;

        Camera* _cam;

        int _terrainWidth, _terrainHeight;

    public:
        SceneManager(){}

        /* Init the scene manager with terrain coordinates, in OpenGL units */
        SceneManager(int terrW, int terrH);

        int AddObject(SceneObject*);

        SceneObject* GetObject(int id) const;
        SceneObject* GetObject(const char* name) const;
        SceneObject* GetObject(float x, float y, float z) const;

        void RemoveObject(SceneObject*);

        Camera* GetCamera() const;
        void SetCamera(Camera*);

        /* Output the camera quadrant int x and y */
        void GetCameraQuadrant(int& x, int& y);

        /* Update the valid objects list.
            This list is responsable to show the valid objects, the objects
            visible to the camera and some beyond
            Return FALSE if we don't have any alteration, TRUE if we have
        */
        bool UpdateValidObjects();

        /* Retrieve the valid objects list */
        std::list<SceneObject*>* GetValidObjects();
    };

}
}

#endif /* end of include guard: SCENEMANAGER_HPP */

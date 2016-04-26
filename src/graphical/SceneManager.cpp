#include "SceneManager.hpp"

using namespace Tribalia::Graphics;

int SceneManager::AddObject(SceneObject* obj)
{
    this->_objects.push_back(obj);
    Log::GetLog()->Write("Added object %s to the SceneManager",
        obj->GetName());
    return 1;
}

SceneObject* SceneManager::GetObject(const char* name) const
{
    for (auto it = _objects.begin(); it != _objects.end(); it++) {
        if (!strcmp((*it)->GetName(), name)) {
            return (*it);
        }
    }

    return NULL;
}

SceneObject* SceneManager::GetObject(float x, float y, float z) const
{
    for (auto it = _objects.begin(); it != _objects.end(); it++) {
        glm::vec3 npos = glm::vec3(x,y,z);
        if ((*it)->GetPosition() == npos) {
            return (*it);
        }
    }

    return NULL;
}

Camera* SceneManager::GetCamera() const
{
    return _cam;
}
void SceneManager::SetCamera(Camera* c)
{
    _cam = c;
}

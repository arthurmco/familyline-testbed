#include "SceneManager.hpp"

using namespace Tribalia::Graphics;

int SceneManager::AddObject(SceneObject* obj)
{
    obj->_id = _objects.size() + 1;
    this->_objects.push_back(obj);
    Log::GetLog()->Write("Added object %s to the SceneManager",
        obj->GetName());
    return 1;
}

SceneObject* SceneManager::GetObject(int id) const
{
    for (auto it = _objects.begin(); it != _objects.end(); it++) {
        if ((*it)->_id == id) {
            return (*it);
        }
    }

    return NULL;
}

SceneObject* SceneManager::GetObject(const char* name) const
{
    for (auto it = _objects.begin(); it != _objects.end(); it++) {
        if (!strcmp((*it)->_name.c_str(), name)) {
            return (*it);
        }
    }

    return NULL;
}

SceneObject* SceneManager::GetObject(float x, float y, float z) const
{
    glm::vec3 npos = glm::vec3(x,y,z);
    for (auto it = _objects.begin(); it != _objects.end(); it++) {
        if ((*it)->_position == npos) {
            return (*it);
        }
    }

    return NULL;
}

void SceneManager::RemoveObject(SceneObject* sco)
{
    glm::vec3 npos = sco->GetPosition();
    for (auto it = _objects.begin(); it != _objects.end(); it++) {
        if ((*it)->_position == npos) {
            if (!strcmp((*it)->_name.c_str(), sco->GetName())) {
                Log::GetLog()->Write("Object %s (%.f %.f %.f) removed from the "
                    "scene", sco->GetName(), npos.x, npos.y, npos.z);
                _objects.erase(it);
            }
        }
    }
}

Camera* SceneManager::GetCamera() const
{
    return _cam;
}
void SceneManager::SetCamera(Camera* c)
{
    _cam = c;
}

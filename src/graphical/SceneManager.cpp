#include "SceneManager.hpp"
#include "LightManager.hpp"

using namespace Tribalia::Graphics;


/* Init the scene manager with terrain coordinates, in OpenGL units */
SceneManager::SceneManager(int terrW, int terrH)
{
    _terrainWidth = terrW;
    _terrainHeight = terrH;

    _quadrants = new SceneQuadrant[ 
        (int)ceil(terrH/SCENE_QUADRANT_SIZE) * (int)ceil(terrW/SCENE_QUADRANT_SIZE) ];
    
}

int SceneManager::AddObject(SceneObject* obj)
{
    obj->_id = _objects.size() + 1 + (uintptr_t)obj;
    this->_objects.push_back(obj);
    Log::GetLog()->Write("scene-manager", "Added object %s (ID %u) to the SceneManager",
        obj->_name.c_str(), obj->_id);
         _listModified = true;
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
     
        if ((*it)->GetID() == sco->GetID()) {
            Log::GetLog()->Write("scene-manager", "Object %s (at %.2f %.2f %.2f) removed from the scene",
				 sco->GetName(), npos.x, npos.y, npos.z);
            _objects.erase(it);
             _listModified = true;
             break;
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

void SceneManager::GetCameraQuadrant(int& x, int& y)
{
    glm::vec3 pos = _cam->GetPosition();
    x = floor(pos.x / SCENE_QUADRANT_SIZE);
    y = floor(pos.z / SCENE_QUADRANT_SIZE);
}

/* Update the valid objects list.
    This list is responsable to show the valid objects, the objects
    visible to the camera and some beyond
    Return FALSE if we don't have any alteration, TRUE if we have
*/
bool SceneManager::UpdateValidObjects()
{
    if (!_listModified) return false;

    this->_valid_objects.clear();
    for (auto it = _objects.begin(); it != _objects.end(); it++) {
        this->_valid_objects.push_back(*it);
    }

	auto lights = LightManager::GetBestLights(_cam->GetPosition(), 128, 3);
	this->_valid_objects.insert(this->_valid_objects.end(), lights.begin(), lights.end());
    
    _listModified = false;
     return true;
}

/* Retrieve the valid objects list */
std::list<SceneObject*>* SceneManager::GetValidObjects()
{
    return &this->_valid_objects;
}

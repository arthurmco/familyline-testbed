#include "MaterialManager.hpp"

using namespace Tribalia::Graphics;

MaterialManager* MaterialManager::_mm = nullptr;

/* Add material, return its ID */
int MaterialManager::AddMaterial(Material* m)
{
    int id = _materials.size();
    m->_ID = id;

    Log::GetLog()->Write("Added material %s (id %d) ["
        " diffuse (%.2f %.2f %.2f) "
        " ambient (%.2f %.2f %.2f) ]",
        m->GetName(), m->GetID(),
        m->GetData()->diffuseColor.r, m->GetData()->diffuseColor.g,
        m->GetData()->diffuseColor.b,
        m->GetData()->ambientColor.r, m->GetData()->ambientColor.g,
        m->GetData()->ambientColor.b);
;
    _materials.push_back(*m);
	return id;
}

/* Add materials*/
void MaterialManager::AddMaterials(std::vector<Material*> materials)
{
    for (int i = 0; i < materials.size(); i++) {
        this->AddMaterial(materials[i]);
    }
}

Material* MaterialManager::GetMaterial(int ID)
{
    for (auto it = _materials.begin(); it != _materials.end(); ++it) {
        if (it->_ID == ID) {
            return &(*it);
        }
    }

    return nullptr;
}

Material* MaterialManager::GetMaterial(const char* name)
{
    for (auto it = _materials.begin(); it != _materials.end(); ++it) {
        if (!strcmp(it->GetName(), name)) {
            return &(*it);
        }
    }

    return nullptr;
}

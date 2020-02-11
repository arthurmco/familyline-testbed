#include <client/graphical/material_manager.hpp>
#include <common/Log.hpp>

using namespace familyline::graphics;

MaterialManager* MaterialManager::_mm = nullptr;

/* Add material, return its ID */
int MaterialManager::addMaterial(Material* m)
{
    int id = _materials.size();
    m->_ID = id;

    Log::GetLog()->Write("material-manager", "Added material %s (id %d) ["
        " diffuse (%.2f %.2f %.2f) "
        " ambient (%.2f %.2f %.2f) ]",
        m->getName(), m->getID(),
        m->getData().diffuseColor.r, m->getData().diffuseColor.g,
        m->getData().diffuseColor.b,
        m->getData().ambientColor.r, m->getData().ambientColor.g,
        m->getData().ambientColor.b);

    printf("<<< %s %d >>>\n", m->getName(), m->getID());
    _materials.push_back(*m);
	return id;
}

/* Add materials*/
void MaterialManager::addMaterials(std::vector<Material*> materials)
{
    for (unsigned i = 0; i < materials.size(); i++) {
        this->addMaterial(materials[i]);
    }
}

Material* MaterialManager::getMaterial(int ID)
{
    for (auto it = _materials.begin(); it != _materials.end(); ++it) {
        if (it->_ID == ID) {
            return &(*it);
        }
    }

    return nullptr;
}

Material* MaterialManager::getMaterial(const char* name)
{
    for (auto it = _materials.begin(); it != _materials.end(); ++it) {
        if (!strcmp(it->getName(), name)) {
            return &(*it);
        }
    }

    return nullptr;
}

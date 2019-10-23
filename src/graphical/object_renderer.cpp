#include "object_renderer.hpp"
#include "mesh.hpp"

using namespace familyline::logic;
using namespace familyline::graphics;

void ObjectRenderer::add(GameObject* const o)
{
    this->components.emplace_back(o->getID(), o);

    std::shared_ptr<Mesh> m = std::dynamic_pointer_cast<Mesh>(
        o->getLocationComponent().value().mesh);
    this->components[o->getID()].meshHandle = _sr.add(m);

    printf("added mesh handle %x\n", this->components[o->getID()].meshHandle);
    
}

void ObjectRenderer::remove(object_id_t id)
{
	//this->components.erase(id);
}

void ObjectRenderer::update()
{
	for (auto& l : this->components) {
        if (l.component->getLocationComponent()) {
            l.component->getLocationComponent()->updateMesh(_terrain);
        }
	}
}

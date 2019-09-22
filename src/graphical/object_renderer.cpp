#include "object_renderer.hpp"
#include "mesh.hpp"

using namespace familyline::logic;
using namespace familyline::graphics;

void ObjectRenderer::add(GameObject* const o)
{
    this->components.emplace_back(o->getID(), o);
}

void ObjectRenderer::remove(object_id_t id)
{
	//this->components.erase(id);
}

void ObjectRenderer::update()
{
	for (auto& l : this->components) {
        std::shared<Mesh> m = std::dynamic_pointer_cast<Mesh>(l.component->mesh);
        if (m) {
            m->
        }
	}
}

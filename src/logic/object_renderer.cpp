#include "object_renderer.hpp"
#include <algorithm>

using namespace familyline::logic;

void ObjectRenderer::add(GameObject& o)
{
	auto lc = o.getLocationComponent();

	if (lc) {
		this->components.emplace_back(o.getID(), lc.value());
	}
}

void ObjectRenderer::remove(object_id_t id)
{
	auto rit = std::remove_if(this->components.begin(), this->components.end(),
		[id](RendererSlot v) {
		return v.id == id;
	});

	this->components.erase(rit, this->components.end());
}

void ObjectRenderer::update()
{
	for (auto& l : this->components) {
		l.component.updateMesh(_terrain, l.component.object->getPosition());
	}
}

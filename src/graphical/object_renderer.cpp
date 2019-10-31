#include "object_renderer.hpp"
#include "mesh.hpp"
#include "../logic/logic_service.hpp"

using namespace familyline::logic;
using namespace familyline::graphics;

void ObjectRenderer::add(GameObject* const o)
{
    if (!o->getLocationComponent()) {
        return;
    }

    RendererSlot rs(o->getID(), o);
    std::shared_ptr<Mesh> m = std::dynamic_pointer_cast<Mesh>(
        o->getLocationComponent().value().mesh);
    rs.meshHandle = _sr.add(m);

    this->components.push_back(rs);
    printf("added mesh handle %x\n", rs.meshHandle);
    
}

void ObjectRenderer::remove(object_id_t id)
{
	//this->components.erase(id);
}

void ObjectRenderer::update()
{
	for (auto& l : this->components) {
        if (l.component->getLocationComponent()) {

            auto* o = l.component;
            glm::vec3 pstart = o->getPosition();
            glm::vec3 pend = o->getPosition();

            pstart += glm::vec3(-5, 5, -5);
            pend += glm::vec3(5, 5, 5);    
            LogicService::getDebugDrawer()->drawSquare(
                pstart, pend, glm::vec4(1, 0.2, 0.1, 1), glm::vec4(0,0, 0,0));
    
            
            l.component->getLocationComponent()->updateMesh(_terrain);
        }
	}
}

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

#include "TerrainRenderer.hpp"

void ObjectRenderer::update()
{
	for (auto& l : this->components) {
        if (l.component->getLocationComponent()) {

            auto* o = l.component;
            glm::vec3 pstart = o->getPosition();
            glm::vec3 pend = o->getPosition();

            std::shared_ptr<Mesh> gmesh = std::dynamic_pointer_cast<Mesh>(
                l.component->getLocationComponent()->mesh);
            BoundingBox bb = gmesh->getBoundingBox();
            glm::vec4 vmin = glm::vec4(bb.minX, bb.minY, bb.minZ, 1);
            glm::vec4 vmax = glm::vec4(bb.maxX, bb.maxY, bb.maxZ, 1);

            glm::vec3 vmin3 = GraphicalToGameSpace(glm::vec3(gmesh->getWorldMatrix() * vmin));
            glm::vec3 vmax3 = GraphicalToGameSpace(glm::vec3(gmesh->getWorldMatrix() * vmax));

            vmin3.y = pstart.y;
            vmax3.y = pend.y;
            vmin3 += glm::vec3(-0.5, 1, -0.5);
            vmax3 += glm::vec3(0.5, 2, 0.5);

            LogicService::getDebugDrawer()->drawSquare(
                vmin3, vmax3, glm::vec4(0.8, 0, 0.5, 1),
                glm::vec4(0,0, 0,0));



            l.component->getLocationComponent()->updateMesh(_terrain);
        }
	}
}

#include "ObjectRenderer.hpp"

using namespace Tribalia::Logic;


ObjectRenderer::ObjectRenderer(ObjectManager* om, Tribalia::Graphics::SceneManager* sm)
    : _om(om), _sm(sm)
{

}

/* Check for new objects, add them to the list */
void ObjectRenderer::Check()
{
    int object_found = 0;
    for (auto it = _om->_objects.begin(); it != _om->_objects.end(); ++it) {

        bool exists = false;
        /* Check if ID already exists.
            If yes, then the object is already added */
        for (auto id_it = _IDs.begin(); id_it != _IDs.end(); ++id_it) {
            if (*id_it == it->oid) {
                exists = true;
                break;
            }
        }

        if (exists) continue;

        /* easy way to check if we have a locatable object
            I don't know if is fast */
        LocatableObject* loc = dynamic_cast<LocatableObject*>(it->obj);
        if (loc) {

            /* check if mesh is valid */
            if (!loc->GetMesh()) continue;

            object_found++;
            _objects.emplace_back(loc);
            _sm->AddObject(loc->GetMesh());
            _IDs.push_back(it->oid);
        }
    }

    if (object_found > 0) {
        Log::GetLog()->Write("Object renderer updated. %d objects found",
            object_found);
    }

}

/* Update object meshes */
void ObjectRenderer::Update()
{
    for (auto it = _objects.begin(); it != _objects.end(); it++) {
        double x, y, z;
        x = (*it)->GetX() * SEC_SIZE;
        y = 0; //change this
        z = (*it)->GetZ() * SEC_SIZE;

        (*it)->GetMesh()->SetPosition(glm::vec3(x,y,z));
        (*it)->GetMesh()->ApplyTransformations();

        printf("\tobject %s id %d is at %.3f %.3f %.3f\n",
            (*it)->GetName(), (*it)->GetObjectID(),
            (*it)->GetMesh()->GetPosition().x,
            (*it)->GetMesh()->GetPosition().y,
            (*it)->GetMesh()->GetPosition().z);

    }
}

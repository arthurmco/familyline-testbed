#include "ObjectRenderer.hpp"

using namespace Tribalia::Logic;


ObjectRenderer::ObjectRenderer(ObjectManager* om, Tribalia::Graphics::SceneManager* sm)
    : _om(om), _sm(sm)
{

}

/*	Check for new objects, add them to the list
	Return true if we have new objects, false if we haven't */
bool ObjectRenderer::Check()
{
    for (auto id_it = _IDs.begin(); id_it != _IDs.end(); ++id_it) {
        id_it->ok = false;        
    }

    int object_found = 0;
    for (auto it = _om->_objects.begin(); it != _om->_objects.end(); ++it) {

        bool exists = false;
        /* Check if ID already exists.
            If yes, then the object is already added */
        for (auto& id_it : _IDs) {
            if (id_it.ID == it->oid) {
                exists = true;
                id_it.ok = true;

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
            ObjectRenderData ord;
            ord.ID = it->oid;
            ord.m = loc->GetMesh();
            ord.ok = true;
            _IDs.push_back(ord);
        }
    }

    if (object_found > 0) {
        Log::GetLog()->Write("Object renderer updated. %d objects found",
            object_found);
        return true;
    }

    for (auto id = _IDs.begin(); id != _IDs.end(); ++id) {
        if (!id->ok) {
            /*  ID is not ok, meaning that it wasn't been updated, meaning
                that it doesn't exist. Remove it from the scene */
            Log::GetLog()->Write("Removed object with id %d", id->ID);
            _sm->RemoveObject(id->m);
            _IDs.erase(id);
            return true;
        }
    }

    /* Check for the inverse */

	return false;

}

/* Update object meshes */
void ObjectRenderer::Update()
{
    for (auto it = _objects.begin(); it != _objects.end(); it++) {
        double x, y, z;
        x = (*it)->GetX();
        y = (*it)->GetY(); //change this
        z = (*it)->GetZ();

        (*it)->GetMesh()->SetPosition(Graphics::TerrainRenderer::GameToGraphicalSpace(glm::vec3(x,y,z)));
        (*it)->GetMesh()->ApplyTransformations();

        /*
        printf("\tobject %s id %d is at %.3f %.3f %.3f\n",
            (*it)->GetName(), (*it)->GetObjectID(),
            (*it)->GetMesh()->GetPosition().x,
            (*it)->GetMesh()->GetPosition().y,
            (*it)->GetMesh()->GetPosition().z);
        */
    }
}



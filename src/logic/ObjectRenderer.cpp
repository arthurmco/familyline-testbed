#include "ObjectRenderer.hpp"

using namespace Tribalia::Logic;


ObjectRenderer::ObjectRenderer(ObjectManager* om, Tribalia::Graphics::SceneManager* sm)
    : GameActionListener("object-renderer-listener"),  _om(om), _sm(sm)
{

}

void ObjectRenderer::OnListen(GameAction& a) {
    /* Register the modified item ID here */
    switch (a.type) {
    case GAT_CREATION:
	this->_created_ids[a.creation.object_id] = true; break;
    case GAT_MOVE:
	this->_modified_ids[a.move.object_id] = true; break;
    case GAT_ATTACK:
	this->_modified_ids[a.attack.attacker_id] = true;
	this->_modified_ids[a.attack.attackee_id] = true;
	break;
    case GAT_DESTROY:
	this->_deleted_ids[a.destroy.object_id] = true;
    }
    
}

/*	Check for new objects, add them to the list
	Return true if we have new objects, false if we haven't */
bool ObjectRenderer::Check()
{
    int object_found = 0;
    int object_deleted = 0;
    for (auto it = _om->_objects.begin(); it != _om->_objects.end(); ++it) {

	bool exists = false;

	// Object was created
	if (_created_ids[it->oid]) {
	    /* Easy way to check if we have a locatable object
	     * I don't worry if it's fast, the compiler might optimize.
	     */
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
		_IDs.push_back(ord);
	    }
	} else {
	    // Object already exists.
	    exists = true;
	}
	
        if (exists) continue;

    }

    if (object_found > 0) {
        Log::GetLog()->Write("object-renderer", "updated, %d objects found",
            object_found);
    }

    for (auto id = _IDs.begin(); id != _IDs.end();) {
	
        if (_deleted_ids[id->ID]) {
	    _deleted_ids[id->ID] = false;
	    object_deleted++;
	    
            /*  ID is not ok, meaning that it wasn't been updated, meaning
                that it doesn't exist. Remove it from the scene */
            Log::GetLog()->Write("object-renderer",
				 "Removed object with id %d", id->ID);
            _sm->RemoveObject(id->m);
            id = _IDs.erase(id);
        } else {
	    id++;
	}
    }

    _modified_ids.clear();
    _created_ids.clear();
    _deleted_ids.clear();
    
    return (object_found > 0) || (object_deleted > 0);
}

/* Update object meshes */
void ObjectRenderer::Update()
{
    for (auto it = _objects.begin(); it != _objects.end(); it++) {
        double x, y, z;
        x = (*it)->GetX();
        y = (*it)->GetY(); 
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



#include "ObjectRenderer.hpp"
#include "../graphical/TerrainRenderer.hpp"

using namespace familyline::logic;


ObjectRenderer::ObjectRenderer(ObjectManager* om, familyline::graphics::SceneManager* sm)
	: GameActionListener("object-renderer-listener"), _om(om), _sm(sm)
{
    ObjectEventEmitter::addListener(&this->oel);
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

	ObjectEvent e;
	while (this->oel.popEvent(e)) {
	    switch (e.type) {
	    case ObjectCreated:
		// Object has been created by someone.
		if (_created_ids[e.oid]) {
		    auto go = e.to;
		    if (go.expired())
			continue;

		    auto sgo = go.lock();
		    if (!sgo->mesh)
			continue;

		    // Object exists and is renderable.
		    // Add the mesh to the scene manager.
		    object_found++;
		    _objects.push_back(go);

		    ObjectRenderData ord;
		    ord.ID = e.oid;
		    ord.m = std::dynamic_pointer_cast<graphics::Mesh>(sgo->mesh);
		    _sm->AddObject(ord.m.get());

		    Log::GetLog()->InfoWrite("object-renderer",
					     "added object id %d", e.oid);
		    _IDs.push_back(ord);
		}
		break;
	    case ObjectDestroyed:
		_deleted_ids[e.oid] = true;
		Log::GetLog()->InfoWrite("object-renderer",
					 "removed object id %d", e.oid);
		_objects.erase(
		    std::remove_if(_objects.begin(), _objects.end(),
				   [&](std::weak_ptr<GameObject>& rgo) {
				       if (rgo.expired())
						   return true;
				       
				       return rgo.lock()->getID() == e.oid;
				   })
		    );
		break;

	    case ObjectCityChanged:
		// TODO: Make something related.
		break;
	    default:
		continue;
	    }
	}

	// !LISTENER
	// for (auto it = _om->_objects.begin(); it != _om->_objects.end(); ++it) {

	// 	bool exists = false;

	// 	// Object was created
	// 	if (_created_ids[it->oid]) {
	// 		/* Easy way to check if we have a locatable object
	// 		 * I don't worry if it's fast, the compiler might optimize.
	// 		 */
	// 		AttackableObject* loc = dynamic_cast<AttackableObject*>(it->obj);
	// 		if (loc) {

	// 			/* check if mesh is valid */
	// 		    if (!loc->mesh) continue;

	// 			object_found++;
	// 			_objects.emplace_back(loc);
	// 			_sm->AddObject((graphics::Mesh*)loc->mesh);
	// 			ObjectRenderData ord;
	// 			ord.ID = it->oid;
	// 			ord.m = (graphics::Mesh*)loc->mesh;
	// 			_IDs.push_back(ord);
	// 		}
	// 	}
	// 	else {
	// 		// Object already exists.
	// 		exists = true;
	// 	}

	// 	if (exists) continue;

	// }

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
			_sm->RemoveObject(id->m.get());
			id = _IDs.erase(id);
		}
		else {
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
    for (auto& obj : _objects) {
	auto sobj = obj.lock();
	if (!sobj)
	    continue;
	
	double x, y, z;
	x = sobj->position.x;
	y = sobj->position.y;
	z = sobj->position.z;

	std::shared_ptr<graphics::Mesh> mm = std::dynamic_pointer_cast<graphics::Mesh>(
	    sobj->mesh);

	mm->SetPosition(graphics::GameToGraphicalSpace(glm::vec3(x, y, z)));
	mm->ApplyTransformations();

	/*
	  printf("\tobject %s id %d is at %.3f %.3f %.3f\n",
	  (*it)->GetName(), (*it)->GetObjectID(),
	  (*it)->GetMesh()->GetPosition().x,
	  (*it)->GetMesh()->GetPosition().y,
	  (*it)->GetMesh()->GetPosition().z);
	*/
    }
}

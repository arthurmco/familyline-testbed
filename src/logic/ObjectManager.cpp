#include "ObjectManager.hpp"

using namespace Familyline;
using namespace Familyline::Logic;

int ObjectManager::AssignID()
{
    /* No free ID? Return the obvious choice */
    if (_freeID.empty())
	return _objects.size();
    
    /* Check the itens on freeID list. Returns the first */
    for (auto it = _freeID.begin(); it != _freeID.end(); ++it){
            int id = *it;
            _freeID.erase(it);
            return id;
    }

    return _objects.size();
}

/* Register an object. Return its ID
    If 'overrideID = true', then the Object Manager will assign a
    new ID.
*/
int ObjectManager::RegisterObject(GameObject* o, bool overrideID)
{
    ObjectRegisterInfo ori;
    ori.obj = o;

    if (overrideID){
        ori.oid = this->AssignID();
    } else {
        ori.oid = o->GetObjectID();
    }

    o->_oid = ori.oid;

    if (!o->Initialize()){
        Log::GetLog()->Write("object-manager", "Initialization of object id %d failed",
            o->_oid);
        return -1; //throw std::game_exception();
    }

    Log::GetLog()->Write("object-manager", "Registered object %s, type %#x, id %d",
        o->_name.c_str(), o->_tid, o->_oid);

    _objects.push_back(ori);
    return ori.oid;



}

/* Unregister an object */
bool ObjectManager::UnregisterObject(GameObject* o)
{
    if ( o && this->UnregisterObject(o->GetObjectID()) ) {
        return true;
    } else {
        Log::GetLog()->Write("object-manager", "Object %s (type %#x, id %d) not found in object manager, so it didn't unregister",
            o->_name.c_str(), o->_tid, o->_oid);
        return false;

    }
}


/* Unregister an object. Return true if the object was there, false
    if it wasn't.
    */
bool ObjectManager::UnregisterObject(int id)
{
    for (auto it = _objects.begin(); it != _objects.end(); ++it){
        /* First search for it */
        if (it->oid == id){
	    _freeID.push_back(it->oid);
	    Log::GetLog()->Write("object-manager", "Unregistered object %s, type %#x, id %d",
				 it->obj->_name.c_str(), it->obj->_tid, it->obj->_oid);
	    
            _objects.erase(it);
            return true;
        }

    }

    /* Not found.  */
    return false;

}


/* Get registered objects */
int ObjectManager::GetCount()
{
    return _objects.size();
}

std::list<ObjectRegisterInfo>* ObjectManager::GetObjectList()
{
	return &_objects;
}

bool ObjectManager::DoActionAll()
{
    bool actions = true;
    for (auto it = _objects.begin(); it != _objects.end(); ++it){
        if (it->obj->DoAction()){
            it->lastiter++;
        } else {
            Log::GetLog()->Write("object-manager",
				 "Object %s (id %d, type %d) returned false in DoAction()", it->obj->GetName(),
            it->obj->GetObjectID(), it->obj->GetTypeID());

        }
    }

    return actions;
}

/* Retrieve an object */
GameObject* ObjectManager::GetObject(int id)
{
    for (auto it = _objects.begin(); it != _objects.end(); ++it) {
        if (it->oid == id) {
            return it->obj;
        }
    }
    return nullptr;
}

GameObject* ObjectManager::GetObject(float x, float y, float z, float bias)
{
    for (auto it = _objects.begin(); it != _objects.end(); ++it) {

        float xPos = it->obj->GetX();
        float yPos = it->obj->GetY();
        float zPos = it->obj->GetZ();

        if (xPos >= (x - bias) && xPos <= (x + bias)) {
            if (yPos >= (y - bias) && yPos <= (y + bias)) {
                if (zPos >= (z - bias) && zPos <= (z + bias)) {
                    return it->obj;
                }
            }
        }
    }

    return nullptr;
}

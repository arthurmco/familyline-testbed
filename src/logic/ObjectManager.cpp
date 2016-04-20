#include "ObjectManager.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;

int ObjectManager::AssignID()
{
    /* Check the itens on freeID list. Returns the first */
    for (auto it = _freeID.begin(); it != _freeID.end(); ++it){
            int id = *it;
            _freeID.erase(it);
            return id;
    }

    /* No free ID? Return the obvious choice */
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
    Log::GetLog()->Write("Registered object %s, type %#x, id %d",
        o->_name.c_str(), o->_tid, o->_oid);

    _objects.push_back(ori);
    return ori.oid;



}

/* Unregister an object */
bool ObjectManager::UnregisterObject(GameObject* o)
{
    for (auto it = _objects.begin(); it != _objects.end(); ++it){
        /* First search for it */
        if (it->oid == o->GetObjectID()){
            _objects.erase(it);
            _freeID.push_back(it->oid);

            Log::GetLog()->Write("Unregistered object %s, type %#x, id %d",
                o->_name.c_str(), o->_tid, o->_oid);


            return true;
        }

    }


    Log::GetLog()->Write("Object %s (type %#x, id %d) not found in "
        "object manager, so it didn't unregister",
        o->_name.c_str(), o->_tid, o->_oid);



    /* Not found.  */
    return false;
}
/* Get registered objects */
int ObjectManager::GetCount()
{
    return _objects.size();
}

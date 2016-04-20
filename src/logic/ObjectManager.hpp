
/***
    Object manager class

    Copyright (C) 2016 Arthur M

***/

#include <list>
#include <vector>
#include "GameObject.hpp"

#include "../Log.hpp"

#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP

namespace Tribalia {
    namespace Logic {

        struct ObjectRegisterInfo {
                int oid;
                GameObject* obj;
        };

        class ObjectManager {
        private:
            std::list<ObjectRegisterInfo> _objects;

            /* List of free IDs to be assigned */
            std::vector<int> _freeID;

            int AssignID();
        public:
            /* Register an object. Return its ID
                If 'overrideID = true', then the Object Manager will assign a
                new ID.
            */
            int RegisterObject(GameObject*, bool overrideID = true);

            /* Unregister an object. Return true if the object was there, false
                if it wasn't*/
            bool UnregisterObject(GameObject*);

            /* Get registered objects */
            int GetCount();
        };

    }
}



#endif /* end of include guard: OBJECTMANAGER_HPP */

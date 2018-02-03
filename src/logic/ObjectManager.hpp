
/***
    Object manager class

    Copyright (C) 2016 Arthur M

***/

#include <list>
#include <vector>
#include "GameObject.hpp"

#include "Log.hpp"

#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP

namespace Tribalia {
    namespace Logic {

        struct ObjectRegisterInfo {
                /* Object ID */
                int oid;

                /* Last iteration time (in 'amount of object loops' ) */
                int lastiter;

                /* Object */
                GameObject* obj;
        };

        class ObjectManager {
            friend class ObjectRenderer;

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
                if it wasn't.

                */
            bool UnregisterObject(GameObject*);

            /* Unregister an object. Return true if the object was there, false
                if it wasn't.
                */
            bool UnregisterObject(int id);

            /* Get registered objects */
            int GetCount();

            /* Run the DoAction() method in each registered object
                TODO: Change this for 'on each visible object' (?)  */
            bool DoActionAll();

            /* Retrieve an object */
            GameObject* GetObject(int id);
            GameObject* GetObject(float x, float y, float z, float bias);
			std::list<ObjectRegisterInfo>* GetObjectList();

        };

    }
}



#endif /* end of include guard: OBJECTMANAGER_HPP */

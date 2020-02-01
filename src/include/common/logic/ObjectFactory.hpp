/*  Represents an object factory
 
    Copyright (C) 2016, 2018 Arthur M
*/

#ifndef OBJECTFACTORY_HPP
#define OBJECTFACTORY_HPP


#include <map>
#include <string>

#include "game_object.hpp"

namespace familyline::logic {


    /**
     * \brief Helper class to instantiate an object without knowing its class
     *
     * Since we will support a lot of objects, objects loaded from files,
     * lua script files, and might support even loading objects from
     * shared libraries (.dll, .so), we need to load them without knowing
     * its class, so the code that use it might use an object without
     * really knowing who it is, only its base class.
     */
    class ObjectFactory {
    private:
        std::map<std::string /*type*/, GameObject*> _objects;

    public:
        /** 
         * Gets an instance of object with type 'typeID', or
         * nullptr if given object wasn't added to the factory
         * i.e, doesn't exist 
         */
        std::shared_ptr<GameObject> GetObject(
            const char* type, float x, float y, float z);

        /** 
         * Adds an object to the factory 
         */
        void AddObject(GameObject* object);

        static ObjectFactory* GetInstance() {
            static ObjectFactory* f = nullptr;
            if (!f)     f = new ObjectFactory;
    
            return f;
        }
    };

}

#endif

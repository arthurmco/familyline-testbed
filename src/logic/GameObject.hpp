
/***
    Game object class implementation

    Copyright 2016 Arthur M.

***/

#include <map>
#include <string>

#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

namespace Tribalia {
    namespace Logic {

    /*  The GameObject class represents the most basic game object

        It contains, however, a hashtable of properties, to allow
        extensibility.
     */

    class GameObject {
        friend class ObjectManager;

    protected:
        int _oid;            /* Object ID */
        int _tid;            /* Type ID */
        std::string _name;   /* Object name */

        /* Property hashtable */
        std::map<std::string, void*> _properties;
        int _xPos, _yPos, _zPos;

    public:
        GameObject(int oid, int tid, const char* name);

        int GetObjectID();
        int GetTypeID();
        const char* GetName();
        void SetName(char*);

        /* Get a property value */
        template<typename T>
        T GetProperty(const char* name)
        {
            std::string sname{name};

            if (_properties.find(sname) == _properties.end()){
                return (T)0;
            }

            return *(T*)_properties[sname];

        }

        /* Find a property */
        bool FindProperty(const char* name)
        {
            std::string sname{name};

            if (_properties.find(sname) == _properties.end()){
                return false;
            }

            return true;

        }

        /* Set a property. Returns false if it doesn't exist */
        template<typename T>
        bool SetProperty(const char* name, T value)
        {
            std::string sname{name};

            if (_properties.find(sname) == _properties.end()){
                return false;
            }

            T* prop = (T*)_properties[sname];
            *prop = value;

            return true;

        }

        /* Add a property. Returns false if it already exists */
        template<typename T>
        bool AddProperty(const char* name, T value)
        {
            std::string sname{name};

            if (_properties.find(sname) == _properties.end()){
                _properties[sname] = new T;
                return this->SetProperty(name, value);

            }


            return false;

        }

        /* Called on object initialization */
        virtual bool Initialize() = 0;

        /* Called on each engine loop, when an action is performed */
        virtual bool DoAction(void) = 0;

    };

    /* Helper macros to ease property handling */
    #define DEF_PROPERTY(name, defaultval) \
        this->AddProperty(name, defaultval)

    #define GET_PROPERTY(type, name) \
        this->GetProperty<type>(name)

    #define SET_PROPERTY(name, val) \
        this->SetProperty(name, val)
    }
}


#endif /* end of include guard: GAMEOBJECT_HPP */

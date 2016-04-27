/***
    City definitions

    Copyright (C) 2016 Arthur M

***/

#include <vector>

#include "AttackableObject.hpp"
#include "../Log.hpp"

#ifndef CITY_HPP
#define CITY_HPP

namespace Tribalia {
namespace Logic {

    struct Team {
        int id;
        std::string name;
        char* flag; //pointer to a raw bitmap.
    };

    class City {
    private:
        std::vector<AttackableObject*> _objects;

        int _men, _women;
        int _boys, _girls;

        std::string _name;
        Team* _team;

    public:
        City(const char* name, Team* team);

        /* Get an object by its ID */
        AttackableObject* GetObject(int ID);

        /* Add object into city. Return ID */
        int AddObject(AttackableObject*);

        /* Remove object from city.
            Returns true if object exists, false otherwise */
        bool RemoveObject(AttackableObject*);

        int CountObjects();

        const char* GetName();
    };


}
}



#endif /* end of include guard: CITY_HPP */

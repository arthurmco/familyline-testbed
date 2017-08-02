#include "City.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;

#include "Team.hpp"

Team::Team(int id, const char* name) {
    this->id = id;
    this->name = std::string(name);
    this->flag = nullptr;
    this->tinfo = (void*)new TeamCities(*this);
    
}

Team::Team() {
    this->id = -1;
    this->name = "";
    this->flag = nullptr;
    this->tinfo = nullptr;
}


City::City(const char* name, Team* team)
        : _name{name}, _team(team)
        {
            Log::GetLog()->Write("city", "City %s created", name);

        }

    /* Get an object by its ID */
    AttackableObject* City::GetObject(int ID)
    {
        for (auto it = _objects.begin(); it != _objects.end(); it++){
            if ((*it)->GetObjectID() == ID){
                return (*it);
            }
        }

        Log::GetLog()->Write("city", "No object with ID %d found on city %s",
            ID, _name.c_str());
        return NULL;
    }

    /* Add object into city. Return ID */
    int City::AddObject(AttackableObject* a){
        _objects.push_back(a);
        a->SetProperty("city", this);
        Log::GetLog()->Write("city", "Added %s (%d) to city %s", a->GetName(), a->GetObjectID(),
            _name.c_str());
        return a->GetObjectID();
    }

    /* Remove object from city.
        Returns true if object exists, false otherwise */
    bool City::RemoveObject(AttackableObject* a){
        for (auto it = _objects.begin(); it != _objects.end(); it++){
            if ((*it)->GetObjectID() == a->GetObjectID()){
                _objects.erase(it);
                return true;
            }
        }

        Log::GetLog()->Write("city", "Tried to delete object %s (ID %d) but city %s doesn't have it",
            a->GetName(), a->GetObjectID(), _name.c_str());
        return false;
    }

    int City::CountObjects(){
        return _objects.size();
    }


const char* City::GetName() const { return _name.c_str(); }
Team* City::GetTeam() const { return _team; }


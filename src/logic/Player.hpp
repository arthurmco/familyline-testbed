/***
    Player base definitions

    Copyright (C) 2016 Arthur M

***/
#include <vector>
#include <cstring> //for strcmp

#include "City.hpp"
#include "GameContext.hpp"

#ifndef PLAYER_HPP
#define PLAYER_HPP

namespace Tribalia {
namespace Logic {

    class Player {

    protected:
        std::string _name;
        int _elo, _xp;
        std::vector<City*> _cities;

    public:
        Player(const char* name, int elo=0, int xp=0);

        City* GetCity();
        City* GetCity(const char* name);

        void AddCity(City*);
        /***
            Virtual function called on each iteration.

            It allows player to decide its movement
            (input for humans, AI decisions for AI... )

            Returns true to continue its loop, false otherwise.
        ***/
        virtual bool Play(GameContext*) = 0;

        /* True if this player has updated an object in the last Play() call */
        virtual bool HasUpdatedObject() = 0;



        const char* GetName();
        int GetXP();

    };


}
}



#endif /* end of include guard: PLAYER_HPP */

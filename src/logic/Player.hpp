
/***
    Player base definitions

    Copyright (C) 2016-2018 Arthur M

***/

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <cstring> //for strcmp

#include "GameContext.hpp"
#include "GameActionManager.hpp"
#include "City.hpp"

namespace Familyline::Logic {

    class Player {
	friend class CityListener;
	
    protected:
        std::string _name;
        int _xp;
        City* _city;

	GameActionManager* _gam;

	// Auxiliar functions to register game actions
	void RegisterCreation(GameObject*);
	void RegisterMove(GameObject*, glm::vec2 position);
	void RegisterAttack(GameObject* attacker, GameObject* attackee);
	void RegisterDestroy(GameObject*);
    public:
        Player(const char* name, int xp=0, GameActionManager* gam = nullptr);

	const char* getName() const { return this->_name.c_str(); }
	int getXP() const { return this->_xp; }
	
        City* GetCity();

        void AddCity(City*);
	
        /***
            Virtual function called on each iteration.

            It allows player to decide its movement
            (input for humans, AI decisions for AI... )

            Returns true to continue its loop, false otherwise.
        ***/
        virtual bool Play(GameContext*) = 0;

	/***
	    Process player input.
	    Returns true if some input was received, false if not 
	***/
	virtual bool ProcessInput() = 0;
	
        /* True if this player has updated an object in the last Play() call */
        virtual bool HasUpdatedObject() = 0;



    };


}


#endif /* end of include guard: PLAYER_HPP */

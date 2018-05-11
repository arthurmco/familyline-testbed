
/***
    Player base definitions

    Copyright (C) 2016, 2017 Arthur M

***/
#include <vector>
#include <cstring> //for strcmp

#include "City.hpp"
#include "GameContext.hpp"
#include "GameActionManager.hpp"

#ifndef PLAYER_HPP
#define PLAYER_HPP

namespace Familyline::Logic {

    class Player {
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


        const char* GetName() const;
        int GetXP() const;

    };


}


#endif /* end of include guard: PLAYER_HPP */

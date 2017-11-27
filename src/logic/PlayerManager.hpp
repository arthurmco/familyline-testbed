/***
    Manage all players in a specific game

    Copyright (C) 2017 Arthur M

***/

#include <list>
#include <algorithm>

#include "Player.hpp"

#ifndef PLAYERMANAGER_HPP
#define PLAYERMANAGER_HPP

namespace Tribalia::Logic {

    struct PlayerData {
	Player* p;
	int ID;
    };

    /*
     * Manages players and is the base for interrelations (sending messages, 
     * resources etc.)
     */
    class PlayerManager {
    private:
	std::list<PlayerData> _playerdata;
	
    public:
	/* Add a player, receive its ID */
	int AddPlayer(Player* p);

	/* Get a player by some information (ID or name) */
	const Player* GetbyID(int ID) const;
	const Player* GetbyName(const char* name) const;
	
    };
    
}

#endif /* PLAYERMANAGER_HPP */


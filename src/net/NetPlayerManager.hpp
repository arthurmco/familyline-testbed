/***
    Network player manager
    Manages player addition and removal, but through a network

    Copyright (C) 2017 Arthur M

***/

#ifndef NETPLAYERMANAGER_HPP
#define NETPLAYERMANAGER_HPP

#include "../logic/PlayerManager.hpp"
#include "../Log.hpp"

namespace Tribalia::Net {

    class NetPlayerManager : public Logic::PlayerManager {
    public:
	
	explicit NetPlayerManager(const char* player_name, int player_id);
	   
	/* Add a player, receive its ID */
	int AddPlayer(Logic::Player* p, int flags = 0);

	/* Get a player by some information (ID or name) */
	const Logic::Player* GetbyID(int ID) const;
	const Logic::Player* GetbyName(const char* name) const;

	/* Process inputs of all players 
	 * Returns true if any input was received
	 */
	bool ProcessInputs();

	/* Play for all users.
	   Return false only if the human player returns false.
	   This usually mean that the human wants to stop the game.
	*/
	bool PlayAll(Logic::GameContext* gct);

	
    };
    
}

#endif /* NETPLAYERMANAGER_HPP */
  

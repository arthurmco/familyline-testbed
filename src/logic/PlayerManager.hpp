
/***
    Manage all players in a specific game

    Copyright (C) 2017 Arthur M

***/

#include <list>
#include <algorithm>

#include "Player.hpp"

#ifndef PLAYERMANAGER_HPP
#define PLAYERMANAGER_HPP

namespace familyline::logic {

    enum PlayerFlags {
	/**
	 * Player is a human 
	 */
	PlayerIsHuman = 1,

	/**
	 * Player is from outside the computer.
	 *
	 * Usually a network player 
	 */
	PlayerIsOutsider = 2,
    };

    struct PlayerData {
	Player* p;
	int ID;
	int flags;
    };

    /**
     * Manages players and is the base for interrelations (sending messages, 
     * resources etc.)
     *
     * \see PlayerFlags, PlayerData
     */
    class PlayerManager {
    private:
	std::list<PlayerData> _playerdata;
	
    public:
	/**
	 * Add a player, receive its ID 
	 */
	int AddPlayer(Player* p, int flags = 0);

	/**
	 * Get a player by some information (ID or name) 
	 */
	const Player* GetbyID(int ID) const;
	const Player* GetbyName(const char* name) const;

	/**
	 * Process inputs of all players 
	 *
	 * \return true if any input was received
	 */
	virtual bool ProcessInputs();

	/**
	 * \brief Play for all users.
	 *
	 * \return false only if the human player returns false.
	 * This usually mean that the human wants to stop the game.
	 */
	bool PlayAll(GameContext* gct);

	virtual ~PlayerManager() {}
    };
    
}

#endif /* PLAYERMANAGER_HPP */


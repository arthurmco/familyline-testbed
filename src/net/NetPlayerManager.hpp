/***
    Network player manager
    Manages player addition and removal, but through a network

    Copyright (C) 2017 Arthur M

***/

#ifndef NETPLAYERMANAGER_HPP
#define NETPLAYERMANAGER_HPP

#include "../logic/PlayerManager.hpp"
#include "Log.hpp"
#include "../HumanPlayer.hpp"
#include "NetPlayerFilter.hpp"

namespace Familyline::Net {
    
    // The net server declaration
    class Server;

    class NetPlayerManager : public Logic::PlayerManager {
    private:
	Logic::PlayerData humandata;
	NetPlayerFilter* npf = nullptr;
	Familyline::Server::NetMessageQueue* smq;
	
    public:

	/**
	 * Build a network player manager.
	 *
	 * @param player_name The local/human player name
	 * @param player_id The local/human player id
	 * @param server_mq The server message queue, to send/receive messages
	 */
	explicit NetPlayerManager(const char* player_name, int player_id,
				  Familyline::Server::NetMessageQueue* server_mq);
	   
	/* Add a player, receive its ID */
	int AddPlayer(Logic::Player* p, int flags = 0);

	/* Get a player by some information (ID or name) */
	const Logic::Player* GetbyID(int ID) const;
	const Logic::Player* GetbyName(const char* name) const;

	/**
	 * Gets information about the remote players
	 */
	void GetRemotePlayers(Server* ns);

	/* Get the human player that represents this client */
	HumanPlayer* GetHumanPlayer();

	/* Gets the player message filter */
	NetPlayerFilter* GetMessageFilter();
	
	/* Process inputs of all players 
	 * Returns true if any input was received
	 */
	virtual bool ProcessInputs();

	/* Play for all users.
	   Return false only if the human player returns false.
	   This usually mean that the human wants to stop the game.
	*/
	bool PlayAll(Logic::GameContext* gct);


	virtual ~NetPlayerManager();
	
    };
    
}

#endif /* NETPLAYERMANAGER_HPP */
  

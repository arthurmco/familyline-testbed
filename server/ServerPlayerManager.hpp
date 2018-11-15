/***
    Player manager for the server

 ***/

#ifndef SERVERPLAYERMANAGER_HPP
#define SERVERPLAYERMANAGER_HPP

#include <list>
#include "Client.hpp"

namespace familyline::Server {

    struct NetPlayer {
	Client* cli;
    };
    

    class PlayerManager {
    private:
	std::list<NetPlayer> _players;

	void SendPlayerList(Client* c);
	void SendMapList(Client* c);
	
	void NotifyPlayerAddition(Client* c);
	
    public:

	/* Add a client as a player
	   Call this function right after the client received its ID */
	void RegisterClient(Client* c);

	/* Process requests made by the clients, like obtaining maps and
	   player lists 
	*/
	void Process();
	

	/*
	  Get the available players
	*/
	const std::list<NetPlayer>& GetPlayers() const {
	    return this->_players;
	}
	
    };
    
}

#endif /* SERVERPLAYERMANAGER_HPP */

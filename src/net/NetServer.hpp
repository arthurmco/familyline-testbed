/*
  Represents a server in the client Tribalia 

  Copyright (C) 2017, 2018 Arthur M
*/

#ifndef NETSERVER_HPP
#define NETSERVER_HPP

#include <vector>
#include <stdexcept>
#include "socket.h"
#include <Log.hpp>
#include "NetPlayerManager.hpp"

#include <NetMessageQueue.hpp>

#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


namespace Tribalia::Net {

    /* Any error that can occur on the server */
    class ServerException : public std::runtime_error {
    public:
	explicit ServerException(const char* err):
	    std::runtime_error(err)
	    {}
    };

    /* Server communication class
       
       Any communication to the server goes on this class

       It also abstracts different clients, receiving messages from different
       clients from one place (the server) and putting them on the clients
    */
    class Server {
    private:
	socket_t _serversock = -1;
	struct sockaddr_in _serveraddr;

	/* Receive a message */
	Tribalia::Server::NetMessageQueue* cmq = nullptr;
	
    public:
	const char* Receive(size_t maxlen = 1024);

	Tribalia::Server::NetMessageQueue* GetQueue() { return this->cmq; }
	
	Server(const char* ipaddr, int port = 12000);

	/* Do the initial communications with the server
	   Throws an exception if the server is not valid
	*/
	void InitCommunications();

	/* Receive messages and put them in the client message queue */
	void GetMessages();

	/**
	 * @brief Retrieves a player manager based on the player information
	 *
	 * Gets the player ID from the server. With this ID, it can know
	 * what messages come from the server and what ones come from the
	 * client. 
	 * Then, with this ID, it constructs the player manager
	 * 
	 * @param playername The local player name
	 */
	NetPlayerManager* GetPlayerManager(const char* playername);

	/**
	 * Notify to the server that you're ready, or not
	 * If 'v' is true, the ready status is set, else it's cleared
	 */
	void SetReady(bool v);

	/**
	 * Check if the server will start the game.
	 *
	 * If this returns true, that means all clients are ready, if returns
	 * false, some client isn't, and it won't start the game
	 */
	bool IsGameStarting() const;
		
	/* Destroy the connection */
	~Server();
    };

}

#endif /* NETSERVER_HPP */


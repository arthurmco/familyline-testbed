/*
  Represents a server in the client Tribalia 

  Copyright (C) 2017 Arthur M
*/

#ifndef NETSERVER_HPP
#define NETSERVER_HPP

#include <vector>
#include <stdexcept>
#include "socket.h"
#include "Log.hpp"
#include "NetPlayerManager.hpp"

#include <ClientMessageQueue.hpp>

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
	Tribalia::Server::ClientMessageQueue* cmq = nullptr;
	
	const char* Receive(size_t maxlen = 1024);

    public:
	Server(const char* ipaddr, int port = 12000);

	/* Do the initial communications with the server
	   Throws an exception if the server is not valid
	*/
	void InitCommunications();

	/* Process all client messages and put them in the corresponding
	   one */
	void ProcessClients();

	/* Retrieve a network player manager */
	NetPlayerManager* GetPlayerManager(const char* playername);

	/* Destroy the connection */
	~Server();
    };

}

#endif /* NETSERVER_HPP */

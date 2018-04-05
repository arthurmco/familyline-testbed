
/***

   Tribalia server manager

   Copyright (C) 2017 Arthur M

***/
#include <vector>
#include <memory>

#include <sys/types.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* for fd access (read(), close(), write()) */
#include <unistd.h>

#include <signal.h>
#include <poll.h>

#else
#define NOMINMAX //prevents Windows from redefining std::min
#include <Windows.h>

#endif

#include <fcntl.h>

#include <errno.h>
#include <cstring>
#include <stdexcept>

#include "Client.hpp"

#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

namespace Tribalia::Server {
    
    class ServerManager {
    private:
	unsigned int port;
	
	socket_t sockfd;
	struct sockaddr_in addr;

	bool started = false;

	std::vector<std::shared_ptr<Client>> clients;

	struct in_addr serv_net_addr_base = {INADDR_ANY};
	short serv_netmask = 32;
        
	/* Check if address is from the same network as the server
	   Return if is not
	*/
	bool CheckAddress(struct in_addr* addr);
	
    public:
        /* Starts a server manager in the specified port */
	ServerManager(int port = 12000);

	/* Set server network address, in form of 
	   ip/netmask (ex: 192.168.1.0/255)
	*/
	void SetNetworkAddress(char* naddr);
	
        /* Retrieve a client, if available
	   If blocks = true, blocks until next client is available.
	   If is false, then return null if no client available
	 */
	Client* RetrieveClient(bool blocks = false);

	/* Update our client so it will be able to receive/send UDP messages.
	   UDP messages carry game objects (constructions, units etc) status
	   changing. TCP messages carries only game control changes and
	   chat.

	   Therefore, making the game UDP available means that the game is
	   about to start

	   Returns true if it could, false if could not.
	*/
	bool EnableUDPClient(Client* c);

	/* Poll for TCP messages and redirect them to the appropriate client */
	void RetrieveTCPMessages();
	
	~ServerManager();

    };

    /* Exception for server-related errors */
    class ServerManagerError : public std::runtime_error {
    public:
	ServerManagerError(const char* what) : std::runtime_error(what)
	    {}

	ServerManagerError(const std::string& what) : std::runtime_error(what)
	    {}

    };


}

#endif /* SERVERMANAGER_HPP */

/***

   Tribalia server manager

   Copyright (C) 2017 Arthur M

***/
#include <sys/types.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* for fd access (read(), close(), write()) */
#include <unistd.h>
#else
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
	
	int sockfd;
	struct sockaddr_in addr;

	bool started = false;
	
    public:
        /* Starts a server manager in the specified port */
	ServerManager(int port = 12000);
	
        /* Retrieve a client, if available
	   If blocks = true, blocks until next client is available.
	   If is false, then return null if no client available
	 */
	Client* RetrieveClient(bool blocks = false);

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

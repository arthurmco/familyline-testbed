/***
 	Representation of a client computer in Tribalia

	Copyright (C) 2016 Arthur M
***/

#include <queue>
#include <algorithm> // for std::min

/* For network comm */
#include <sys/types.h>
#ifndef _WIN32

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef int socket_t;

/* for fd access (read(), close(), write()) */
#include <unistd.h>
#else
#include <Windows.h>
typedef SOCKET socket_t;
#endif

#include <fcntl.h>

#include <errno.h>
#include <cstring>

#ifndef CLIENT_HPP
#define CLIENT_HPP

#define MAX_CLIENT_BUFFER 8192
namespace Tribalia::Server {

    class Client {
    private:
	int sockfd;
	struct in_addr addr;

	size_t buffer_ptr_send = 0, buffer_ptr_recv = 0;
	char buffer[MAX_CLIENT_BUFFER];

	bool closed = true;
	
    public:
	Client(int sockfd, struct in_addr addr);
	
	void Send(char* m);

        /* Returns false if no message received,
 	 * or true if message received, and outputs the message on m */
	bool Receive(char* m, size_t len);

	/* Injects message in the client
	   Only meant to be called from the server */
	void InjectMessage(char* m, size_t len);
	// void InjectMessage(MessagePacket pkt)

	void Close();
	bool IsClosed();

	socket_t GetSocket();
	struct in_addr GetAddress();
    };

}
#endif

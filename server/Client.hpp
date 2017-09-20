/***
 	Representation of a client computer in Tribalia

	Copyright (C) 2016 Arthur M
***/

/* For network comm */
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

#ifndef CLIENT_HPP
#define CLIENT_HPP

namespace Tribalia::Server {

class Client {
private:
	int sockfd;
#ifndef _WIN32
	struct in_addr addr;
#endif
public:
	Client(int sockfd, struct in_addr addr);

	void Send(char* m);
	/* Returns false if no message received,
 	 * or true if message received, and outputs the message on m */
	bool Receive(char* m, size_t len);

	void Close();
};

}
#endif

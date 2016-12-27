/***
 	Representation of a client computer in Tribalia

	Copyright (C) 2016 Arthur M
***/

/* For network comm */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* for fd access (read(), close(), write()) */
#include <unistd.h>

#include <errno.h>
#include <cstring>

#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client {
private:
	int sockfd;
	struct in_addr addr;

public:
	Client(int sockfd, struct in_addr addr);

	void Send(char* m);
	/* Returns false if no message received,
 	 * or true if message received, and outputs the message on m */
	bool Receive(char* m, size_t len);

	void Close();
};

#endif

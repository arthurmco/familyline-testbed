/***
 	Tribalia server entry point	

	Copyright (C) 2016 Arthur M
***/

#include "../src/EnviroDefs.h"

#include <errno.h>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 3456

int main(int argc, char const* argv[]) 
{
	printf("Tribalia Server " VERSION "\n");
	printf("Copyright (C) 2016 Arthur M\n ");
	printf("Listening on port %d\n", PORT);

	/* Create the socket */
	int sockfd = -1;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(addr.sin_zero), 0, 8);
	
	if (bind(sockfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) < 0) {
		fprintf(stderr, "Error while binding socket: %s\n",
						strerror(errno));
		return 1;
	}

	if (listen(sockfd, 0) < 0) {
		fprintf(stderr, "Error while listening socket: %s\n",
						strerror(errno));
		return 1;
	}

	bool connected = true;
	while (connected) {
		struct sockaddr_in cliaddr;
		memset(&cliaddr.sin_zero, 0, 8);
		socklen_t cliaddr_len = sizeof(struct sockaddr_in);
		int clisockfd = accept(sockfd, (struct sockaddr*) &cliaddr, &cliaddr_len);

		if (clisockfd < 0) {
			fprintf(stderr, "Error while accepting socket: %s\n", strerror(errno));
			continue;
		}

		char ipstr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, (void*)&(cliaddr.sin_addr), ipstr, INET_ADDRSTRLEN);

		printf("Socket accepted (fd %d), address %s\n", clisockfd, ipstr);
		close(clisockfd);
	}	

}	

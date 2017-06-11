#include "Client.hpp"

Client::Client(int sockfd, struct in_addr addr)
{
#ifndef _WIN32
	this->sockfd = sockfd;
	this->addr = addr;

	// Set socket to non-blocking
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
#endif
}

void Client::Send(char* m)
{
#ifndef _WIN32
	write(this->sockfd, m, strlen(m));
#endif
}

/* Returns false if no message received,
 * or true if message received, and outputs the message on m */
bool Client::Receive(char* m, size_t len)
{
#ifndef _WIN32
	auto res = read(this->sockfd, m, len);	
	if (res < 0) {
		if (errno == EAGAIN || errno == EINTR) {
			/* 	No data avaliable
			 	Even if we're interrupted, show this as no data avaliable,
				because you can always come back to get the real data.
			 */
			return false;
		}
	}
#endif
	return true;

}


void Client::Close()
{
#ifndef _WIN32
	close(this->sockfd);
#endif
}

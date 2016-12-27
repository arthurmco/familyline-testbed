#include "Client.hpp"


Client::Client(int sockfd, struct in_addr addr)
{
	this->sockfd = sockfd;
	this->addr = addr;

	// Set socket to non-blocking
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

void Client::Send(char* m)
{
	write(this->sockfd, m, strlen(m));
}

/* Returns false if no message received,
 * or true if message received, and outputs the message on m */
bool Client::Receive(char* m, size_t len)
{
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

	return true;
}


void Client::Close()
{
	close(this->sockfd);
}

#include "Client.hpp"
#include <cstdio>
using namespace Tribalia::Server;

Client::Client(int sockfd, struct in_addr addr)
{
    this->sockfd = sockfd;
    this->addr = addr;
    this->closed = false;
}

void Client::Send(char* m)
{
    write(this->sockfd, m, strlen(m));
}

/* Returns false if no message received,
 * or true if message received, and outputs the message on m */
bool Client::Receive(char* m, size_t len)
{
    if (buffer_ptr_send <= buffer_ptr_recv) {
	buffer_ptr_send = buffer_ptr_recv = 0;
	return false;
    }

    len = std::min(len, (buffer_ptr_send - buffer_ptr_recv));
    strncpy(m, &buffer[buffer_ptr_recv], len);

    if (buffer_ptr_recv+len > MAX_CLIENT_BUFFER) {
	fprintf(stderr, "Error: client buffer %d overflow recv\n", this->sockfd);
	return false;
    }

    buffer_ptr_recv += len;
    return true;
}

/* Injects message in the client
   Only meant to be called from the server */
void Client::InjectMessage(char* m, size_t len)
{
    if (buffer_ptr_send <= buffer_ptr_recv) {
	buffer_ptr_send = buffer_ptr_recv = 0;
    }
    
    strncpy(&buffer[buffer_ptr_send], m, len);

    if (buffer_ptr_send+len > MAX_CLIENT_BUFFER) {
	fprintf(stderr, "Error: client buffer %d overflow\n", this->sockfd);
	return;
    }

    buffer_ptr_send += len;
}


void Client::Close()
{
    if (!this->closed) {
	shutdown(this->sockfd, 2);
	close(this->sockfd);
    }

    this->closed = true;
}

bool Client::IsClosed() { return this->closed; }

socket_t Client::GetSocket()
{
    return this->sockfd;
}

#include "Client.hpp"
#include <cstdio>
using namespace Tribalia::Server;

Client::Client(int sockfd, struct in_addr addr)
{
    this->sockfd = sockfd;
    this->addr = addr;

    /* Set client name as its IP */
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, (void*)&addr, ipstr, INET_ADDRSTRLEN);

    char cname[INET_ADDRSTRLEN+6];
    sprintf(cname, "<<%s>>", ipstr);
    this->name = std::string{cname};
        
    this->closed = false;
}

void Client::SendTCP(const char* m)
{
    if (!this->closed)
	write(this->sockfd, m, strlen(m));
}


/* 'Peek' a message, i.e read but not remove it from the queue 
   Return message length or 0 if no message received
*/
size_t Client::PeekTCP(char* m, size_t len) {
        if (buffer_ptr_send <= buffer_ptr_recv) {
	buffer_ptr_send = buffer_ptr_recv = 0;
	return 0;
    }

    if (this->check_headers) {
	// try to find the starting message token
	const char start_token = '[';
	bool st_token_found = false;

	do {
	    if (buffer[buffer_ptr_recv] == start_token) {
		st_token_found = true;
		break;
	    }

	    buffer_ptr_recv++;
	} while (buffer_ptr_recv < buffer_ptr_send);

	if (!st_token_found) {
	    return false;
	}
    }

    len = std::min(len, (buffer_ptr_send - buffer_ptr_recv));
    strncpy(m, &buffer[buffer_ptr_recv], len);

    /* Tries to end a message on the end token */
    const char end_token = ']';
    bool token_found = false;
    char* tokpos = m;
    do {
	char* ntok = strchr(tokpos, end_token);
	if (!ntok) {
	    break;  // no token found, end it anyway...
	}
	
	if (*(ntok - 1) != '\\') {
	    // If token isn't escaped, end it.
	    ntok++;
	    len = size_t(ntok - m);
	    *ntok = '\0';
	    token_found = true;
	}

	tokpos = ++ntok;
	
    } while (!token_found);
    
    return len;
}

/* Returns 0 if no message received,
 * or strlen(message) if message received, and outputs the message on m */
size_t Client::ReceiveTCP(char* m, size_t len)
{
    auto peek = this->PeekTCP(m, len);
    
    if (buffer_ptr_recv+peek > MAX_CLIENT_BUFFER) {
	fprintf(stderr, "Error: client buffer %d overflow recv\n", this->sockfd);
	return 0;
    }

    if (peek > 0)
	buffer_ptr_recv += peek;
    
    return peek;
}

/* Injects message in the client
   Only meant to be called from the server */
void Client::InjectMessageTCP(char* m, size_t len)
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

bool Client::IsClosed() const { return this->closed; }

socket_t Client::GetSocket() const { return this->sockfd; }
struct in_addr Client::GetAddress() const { return this->addr; }


bool Client::CheckHeaders() const  { return this->check_headers; }
void Client::SetCheckHeaders(bool val) { this->check_headers = val; }

ConnectionStatus Client::GetStatus() const { return this->cstatus; }

void Client::AdvanceStatus() {
    if (this->cstatus <= CS_INGAME)
	this->cstatus = ConnectionStatus(int(this->cstatus) + 1);
}

const char*  Client::GetName() const { return this->name.c_str(); }
void  Client::SetName(char* n) { this->name = std::string{n}; }

unsigned int Client::GetID() const {
/*    unsigned long long l =  (unsigned long long)this +
	(unsigned long long)buffer;

	return (unsigned int)((l & 0xffffffff) + (l << 32)); */
    return strlen(this->name.c_str());
}

#include "NetMessageQueue.hpp"

using namespace Tribalia::Server;

NetMessageQueue::NetMessageQueue(socket_t sock, struct in_addr addr)
    : sockfd(sock), addr(addr)
{}


void NetMessageQueue::SendTCP(const char* m)
{
    write(this->sockfd, m, strlen(m));
}

/* Set/get if the server manager will check the headers of this message
   (i.e, if it starts with '[TRIBALIA') */
bool NetMessageQueue::CheckHeaders() const
{
    return check_headers;
}

void NetMessageQueue::SetCheckHeaders(bool val)
{
    check_headers = val;
}


/* 'Peek' a message, i.e read but not remove it from the queue 
   Return message length or 0 if no message received
*/
size_t NetMessageQueue::PeekTCP(char* m, size_t len) {
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
size_t NetMessageQueue::ReceiveTCP(char* m, size_t len)
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
void NetMessageQueue::InjectMessageTCP(const char* m, size_t len)
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




NetMessageQueue::~NetMessageQueue()
{
    shutdown(this->sockfd, 2);
    close(this->sockfd);
	
    if (this->udp_init) {
	shutdown(this->udp_socket, 2);
	close(this->udp_socket);
	this->udp_init = false;
    }

}

void NetMessageQueue::SendUDP(UDPMessage m)
{
    udp_buffer_send.push(m);
}

bool NetMessageQueue::PeekUDP(UDPMessage& m)
{
    if (udp_buffer_recv.size() == 0 || !udp_init)
	return false;

    auto msg = udp_buffer_recv.front();
    m = msg;
    return true;
}

bool NetMessageQueue::ReceiveUDP(UDPMessage& m)
{
    auto r = this->PeekUDP(m);
    if (!r)
	return false;

    udp_buffer_recv.pop();
    return true;
}

/* Injects UDP messages
   Needs to have the checksum checked */
void NetMessageQueue::InjectMessageUDP(UDPMessage m)
{
    udp_buffer_recv.push(m);
}

bool NetMessageQueue::RetrieveSendUDP(UDPMessage& m)
{
    if (udp_buffer_send.size() == 0 || !udp_init)
	return false;

    auto msg = udp_buffer_send.front();
    m = msg;
    udp_buffer_send.pop();
    return true;
}

/* Initialize the "UDP part" of the client */
bool NetMessageQueue::InitUDP(struct in_addr udp_addr, int port)
{
    memset((void*)&this->udp_addr, 0, sizeof(this->udp_addr));
    this->udp_addr.sin_family = AF_INET;
    this->udp_addr.sin_port = htons(port);
    this->udp_addr.sin_addr = udp_addr;

    this->udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->udp_socket < 0)
	return false;

    if (bind(this->udp_socket, (struct sockaddr*)&this->udp_addr,
	     sizeof(this->udp_addr)) < 0) {
	return false;
    }

    this->udp_init = true;
    return true;
}

bool NetMessageQueue::CheckUDP() const
{
    return this->udp_init;
}

socket_t NetMessageQueue::GetUDPSocket() const
{
    return this->udp_socket;
}

socket_t NetMessageQueue::GetSocket() const
{
    return this->sockfd;
}

struct in_addr NetMessageQueue::GetAddress() const
{
    return this->addr;
}

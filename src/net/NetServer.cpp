#include "NetServer.hpp"

using namespace Tribalia::Net;

/* Build the server socket */
Server::Server(const char* ipaddr, int port)
{
    // Create the socket
    _serversock = socket(AF_INET, SOCK_STREAM, 0);
    if (_serversock < 0) {
	char* s_strerror = strerror(errno);
	char err[256 + strlen(s_strerror)];
	sprintf(err, "error while creating socket: %s", s_strerror);
	throw ServerException(err);
    }

    memset(&_serveraddr, 0, sizeof(struct sockaddr_in));
    _serveraddr.sin_family = AF_INET;
    _serveraddr.sin_port = htons((short)port);
    inet_pton(AF_INET, ipaddr, &(_serveraddr.sin_addr));
    if (connect(_serversock, (struct sockaddr*)&_serveraddr,
		sizeof(struct sockaddr_in)) < 0) {
	char* s_strerror = strerror(errno);
	char err[256 + strlen(s_strerror)];
	sprintf(err, "error while attempting connection: %s", s_strerror);
	throw ServerException(err);
    }    
	
}

/* Receive a message */
const char* Server::Receive()
{
    // Maximum recv loops before closing all
    constexpr int max_recv = 8;

    char ret[1024];
    ssize_t slen = 0;

    for (int i = 0; i < max_recv; i++) {
	slen = recv(_serversock, &ret[i*128], 128, 0);
	if (slen == 0) {
	    Log::GetLog()->Warning("net-server",
				   "Server was shut down unexpectedly");
	    throw ServerException("Server was shut down unexpectedly");
	    return nullptr;
	}

	if (slen < 0) {
	    char* s_strerror = strerror(errno);
	    char err[256 + strlen(s_strerror)];
	    sprintf(err, "error while receiving message: %s", s_strerror);
	    Log::GetLog()->Fatal("net-server", err);
	    throw ServerException(err);
	}

	if (slen <= 128) {
	    // Validate the message end
	    if (ret[(i*128)+slen-1] == '\n' &&
		ret[(i*128)+slen-2] == ']') {
		
		ret[(i*128)+slen] = '\0';
		return strdup(ret);
	    }
	}
	
    }

    Log::GetLog()->Warning("net-server",
			   "received message not fully ended");
    return strdup(ret);
}

/* Do the initial communications with the server
   Throws an exception if the server is not valid
*/
void Server::InitCommunications()
{
    /* Do the communications in a blocking way */
    write(_serversock, "[TRIBALIA CONNECT]\n", 21);

    const char* recv1 = this->Receive();
    int ipelems[4];

    if (strncmp(recv1, "[TRIBALIA CONNECT", 17)) {
	throw ServerException("Unexpected message: step1");
    }
    auto scanv = sscanf(&recv1[17], "%d.%d.%d.%d", &ipelems[0],
			&ipelems[1], &ipelems[2], &ipelems[3]);

    if (scanv < 4) {
	throw ServerException("server: Unexpected IP address");
    }
    
    printf("Server IP is %d.%d.%d.%d\n",
	   ipelems[0], ipelems[1], ipelems[2], ipelems[3]);
    write(_serversock, "[TRIBALIA CONNECT OK]\n", 23);
    free((void*)recv1);

    recv1 = this->Receive();
    if (strncmp(recv1, "[TRIBALIA VERSION?]", 19)) {
	printf("real msg: %s\n", recv1);
	throw ServerException("Unexpected message: step2");
    }
    free((void*)recv1);

    write(_serversock, "[TRIBALIA VERSION 0.1]\n", 24);
    usleep(10);
    write(_serversock, "[TRIBALIA CAPS?]\n", 19);

    
    recv1 = this->Receive();
    if (strncmp(recv1, "[TRIBALIA CAPS", 14)) {
	throw ServerException("Unexpected message: step3");
    }

    // Parse capabilities.
    // Clone the string and parse the caps
    char* caps = strdup(&recv1[15]);
    free((void*)recv1);

    char* caps1end = strstr(caps, "]\n\0");
    if (caps1end)
	*caps1end = '\0';

    std::vector<char*> capslist, requiredcapslist;
    char* cap = strtok(caps, " ");
    while (cap) {
	if (cap[0] == '(')
	    requiredcapslist.push_back(cap);
	else
	    capslist.push_back(cap);
	
	cap = strtok(nullptr, " ");
    }

    if (requiredcapslist.size() > 0) {
	for (auto cap : requiredcapslist) {
	    Log::GetLog()->Fatal("net-server",
				 "Unsupported capability: %s", cap);
	}
	
	throw ServerException("Server required for some capabilities we "
	    "don't support");
    }

    write(_serversock, "[TRIBALIA CAPS ]", 18);
}

/* Process all client messages and put them in the corresponding
   one */
void Server::ProcessClients()
{
    
    
}

/* Retrieve a network player */
NetPlayerManager* Server::GetPlayer()
{
    return new NetPlayerManager();
}

/* Destroy the connection */
Server::~Server()
{
    if (_serversock >= 0) {
	shutdown(_serversock, SHUT_RDWR);
	close(_serversock);
	_serversock = -1;
    }
}

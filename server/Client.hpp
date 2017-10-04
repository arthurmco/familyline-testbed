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
#include <string>

#ifndef CLIENT_HPP
#define CLIENT_HPP

#define MAX_CLIENT_BUFFER 8192
namespace Tribalia::Server {

    /* Define the connection status, obviously */
    enum ConnectionStatus {
	/* Initial disconnected status */
	CS_DISCONNECTED = 0,
	
	/* Stabilishing initial communications, determining if
	   client is really valid and what it does offer */
	CS_CONNECTING,

	/* Client connected, but no game started
	   Usually means fully TCP connected, but no UDP, since
	   UDP is used for in-game commands */
	CS_CONNECTED,

	/* Stabilishing initial game configurations, aka UDP communications */
	CS_GAMESTARTING,

	/* Fully connected */
	CS_INGAME
    };

    class Client {
    private:
	int sockfd;
	struct in_addr addr;

	size_t buffer_ptr_send = 0, buffer_ptr_recv = 0;
	char buffer[MAX_CLIENT_BUFFER];

	bool closed = true;
	
	bool check_headers = true;

	ConnectionStatus cstatus = CS_DISCONNECTED;

	std::string name;
	
    public:
	Client(int sockfd, struct in_addr addr);
	
	void SendTCP(const char* m);

        /* Returns false if no message received,
 	 * or true if message received, and outputs the message on m */
	bool ReceiveTCP(char* m, size_t len);
  

	/* Injects message in the client
	   Only meant to be called from the server */
	void InjectMessageTCP(char* m, size_t len);
	// void InjectMessage(MessagePacket pkt)

	void Close();
	bool IsClosed() const;

	socket_t GetSocket() const;
	struct in_addr GetAddress() const;

	/* Set/get if the server manager will check the headers of this message
	   (i.e, if it starts with '[TRIBALIA') */
	bool CheckHeaders() const;
	void SetCheckHeaders(bool);


	/* Get the connection status 
	   The connection status will update itself according with 
	   the received messages in TCP front	  
	 */
	ConnectionStatus GetStatus() const;

	void AdvanceStatus();

	const char* GetName() const;
	void SetName(char* n);
	
    };

    /* TCP initialization steps.
       They are sequencial and pratically autodescriptive */
    enum TCPInitStep {
	InitConnect,
	VersionQuery,
	VersionQueried,
	CapabilityQuery,
	CapabilityQueried,
	PlayerInfoRetrieve,
	PlayerInfoRetrieved,
	ClientReady,
    };

    /* Structure for keep track of client initialization steps */
    struct TCPInit {
	int step;
	Client* cli;
	int iters = 0;

	TCPInit(Client*);
    };

    /* Manages client initialization on TCP site, such as capability 
       querying, and something like that.
       
       Clients will be auto-removed when they reach the "Connected" state.
    */
    class TCPConnectionInitiator {
    private:
	// Clients that are being initiated
	std::vector<TCPInit> _initClients;

    public:
	/* Add a client to the TCP client initialization. */
	void AddClient(Client*);

	/* Process each step of client initialization */
	void Process();

	/* Check if we have clients to be initialized */
	bool HasClient();
	
    };


}
#endif

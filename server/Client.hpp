/***
 	Representation of a client computer in Tribalia

	Copyright (C) 2016 Arthur M
***/

#include <cstddef> // strict byte size

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

    /* UDP message header layout, little endian */
    struct UDPMessageHeader {
	/* The magic 'number', "TRMP" */
	uint32_t magic;

	/* Packet ID. 
	   Here, will be chosen sequentially by the server manager, but do not
	   need to be 
	*/
	uint32_t packet_id; 

	/* The 64 bit unix timestamp of the message */
	uint64_t packet_timestamp;

	/* Packet checksum. 
	   32-bit sum of all bytes of the packet, */
	uint32_t checksum;

        uint16_t type;
	uint16_t flags;
	uint16_t size;
    } __attribute__((packed));

    struct UDPMessage {
	UDPMessageHeader hdr;
	char* content;
    };

    class Client {
    protected:
	socket_t sockfd;
	struct in_addr addr;

	struct sockaddr_in udp_addr;
	bool udp_init = false;
	socket_t udp_socket;

	size_t buffer_ptr_send = 0, buffer_ptr_recv = 0;
	char buffer[MAX_CLIENT_BUFFER];

	std::queue<UDPMessage> udp_buffer_send, udp_buffer_recv;

	bool closed = true;
	
	bool check_headers = true;

	bool _is_ready = false;

	ConnectionStatus cstatus = CS_DISCONNECTED;

	std::string name;
    public:
	Client(int sockfd, struct in_addr addr);
	
	void SendTCP(const char* m);
	
        /* 'Peek' a message, i.e read but not remove it from the queue 
           Return message length or 0 if no message received */	
	size_t PeekTCP(char* m, size_t len);
	
        /* Returns false if no message received,
 	 * or true if message received, and outputs the message on m */
        size_t ReceiveTCP(char* m, size_t len);
	/* Injects message in the client
	   Only meant to be called from the server */
	void InjectMessageTCP(char* m, size_t len);

	/* Initialize the "UDP part" of the client */
	bool InitUDP(struct in_addr udp_addr, int port);
	bool CheckUDP() const;
	
	void SendUDP(UDPMessage m);
	bool PeekUDP(UDPMessage& m);
	bool ReceiveUDP(UDPMessage& m);

	/* Injects UDP messages
	   Needs to have the checksum checked */
	void InjectMessageUDP(UDPMessage m);
	bool RetrieveSendUDP(UDPMessage& m);

	void Close();
	bool IsClosed() const;

	socket_t GetSocket() const;
	socket_t GetUDPSocket() const;
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

	unsigned int GetID() const;

	/* Set if the client sent the message that it is ready to 
	   start the game  
	*/
	void SetReady() { _is_ready = true; }
	void UnsetReady() { _is_ready = false; }
	bool IsReady() const { return _is_ready; }
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

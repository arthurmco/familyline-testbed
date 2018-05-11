
/***
    Message queue controlling class for the client

    Copyright (C) 2017,2018 Arthur M
***/

#ifndef CLIENTMESSAGEQUEUE_HPP
#define CLIENTMESSAGEQUEUE_HPP

#include <queue>

/* For network comm */
#include <sys/types.h>

#include <cstring>
#include <string>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/* for fd access (read(), close(), write()) */
#include <unistd.h>

#else
#define NOMINMAX //prevents Windows from redefining std::min

#include <io.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define SHUT_RDWR SD_BOTH // I think Microsoft does these things on purpose...

#define usleep(x) Sleep(x/1000);
#endif


#include <ClientUDPMessage.hpp>
#include "../src/net/socket.h"

#define MAX_CLIENT_BUFFER 8192

namespace Familyline::Server {

    
/*
  Manages the messages and connection of the client
*/
    class NetMessageQueue {
    private:
	socket_t sockfd;
	struct in_addr addr;

	struct sockaddr_in udp_addr;
	bool udp_init = false;
	socket_t udp_socket;

	size_t buffer_ptr_send = 0, buffer_ptr_recv = 0;
	char buffer[MAX_CLIENT_BUFFER];

	std::queue<UDPMessage> udp_buffer_send, udp_buffer_recv;
	
	bool check_headers = true;
	
    public:
	NetMessageQueue(socket_t, struct in_addr);

	/* Set/get if the server manager will check the headers of this message
	   (i.e, if it starts with '[TRIBALIA') */
	bool CheckHeaders() const;
	void SetCheckHeaders(bool);

	void SendTCP(const char* m);
	
	/* 'Peek' a message, i.e read but not remove it from the queue 
	   Return message length or 0 if no message received */	
	size_t PeekTCP(char* m, size_t len);
	
	/* Returns false if no message received,
	 * or true if message received, and outputs the message on m */
	size_t ReceiveTCP(char* m, size_t len);
	/* Injects message in the client
	   Only meant to be called from the server */
	void InjectMessageTCP(const char* m, size_t len);

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

	socket_t GetSocket() const;
	socket_t GetUDPSocket() const;
	struct in_addr GetAddress() const;

        ~NetMessageQueue();

    };


}

#endif /* CLIENTMESSAGEQUEUE_HPP */

/***
 	Representation of a client computer in Tribalia

	Copyright (C) 2016, 2017 Arthur M
***/

#include <cstddef> // strict byte size

#include <queue>
#include <algorithm> // for std::min
#include <ClientMessageQueue.hpp>

#include <fcntl.h>
#include <string>

#include <errno.h>

#ifndef CLIENT_HPP
#define CLIENT_HPP

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
    protected:
	bool closed = true;
	ClientMessageQueue* _cmq;

	bool _is_ready = false;

	ConnectionStatus cstatus = CS_DISCONNECTED;

	std::string name;
    public:
	Client(int sockfd, struct in_addr addr);

	void Close();
	bool IsClosed() const;

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

	/* Gets the message queue of this client */
	ClientMessageQueue* GetQueue();

	~Client();
    };


}
#endif

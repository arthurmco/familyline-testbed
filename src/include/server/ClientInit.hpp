/***
    Client initialisation class 

    Copyright (C) 2017 Arthur M
***/

#ifndef CLIENTINIT_HPP
#define CLIENTINIT_HPP

#include <vector>
#include "Client.hpp"
#include "ServerPlayerManager.hpp"

namespace familyline::Server {


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

	PlayerManager* _pm;
    public:
	TCPConnectionInitiator(PlayerManager*);

	/* Add a client to the TCP client initialization. */
	void AddClient(Client*);

	/* Process each step of client initialization */
	void Process();

	/* Check if we have clients to be initialized */
	bool HasClient();
    };


}

#endif /* CLIENTINIT_HPP */

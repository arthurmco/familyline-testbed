/* Client initialization procedures
   Copyright (C) 2017 Arthur M
*/

#include "Client.hpp"
#include <cstdio>

using namespace Tribalia::Server;

TCPInit::TCPInit(Client* c)
{
    this->step = TCPInitStep::InitConnect;
    this->cli = c;
}

/* Add a client to the TCP client initialization. */
void TCPConnectionInitiator::AddClient(Client* c)
{
    if (c->GetStatus() == CS_DISCONNECTED)
	c->AdvanceStatus();
	    
    _initClients.push_back(TCPInit(c));
}

/* Process each step of client initialization */
void TCPConnectionInitiator::Process()
{
    if (_initClients.size() == 0)
	return;
    
    char recvbuffer[256];
    
    for (auto& tinit : _initClients) {
	tinit.iters++;	   
	switch (tinit.step) {
	case InitConnect:
	    if (!tinit.cli->ReceiveTCP(recvbuffer, 128)) {
		continue;
	    }
	    
	    if (!strcmp(recvbuffer, "[TRIBALIA CONNECT OK]")) {
		tinit.step = TCPInitStep::VersionQuery;
	    }
	    
	    break;
	case VersionQuery:
	    tinit.cli->SendTCP("[TRIBALIA VERSION?]\n");
	    tinit.step = TCPInitStep::VersionQueried;
	    break;
	case VersionQueried:
	    if (!tinit.cli->ReceiveTCP(recvbuffer, 128)) {
		continue;
	    }
	    
	    {
		char sname[32], sver[32];
		unsigned int vmaj = 0, vmin = 0;
		int ret = sscanf(recvbuffer, "[%s %s %u.%u",
				 sname, sver, &vmaj, &vmin);

		if (strcmp(sname, "TRIBALIA") ||
		    strcmp(sver, "VERSION")) {
		    continue;
		}
	        
		if (ret < 4) {
		    continue;
		}

		if (vmaj != 0 || vmin != 1) {
		    fprintf(stderr, "Client %s has unsupported version (%u.%u)",
			    tinit.cli->GetName(), vmaj, vmin);
		    tinit.cli->Close();
		    continue;	    
		}

		tinit.step = TCPInitStep::CapabilityQuery;
	    }
	    break;
	case CapabilityQuery:
	    tinit.cli->SendTCP("[TRIBALIA CAPS?]\n");
	    tinit.step = TCPInitStep::CapabilityQueried;
	    break;
	case CapabilityQueried:
	    if (!tinit.cli->ReceiveTCP(recvbuffer, 128)) {
		continue;
	    }

	    {
		char sname[32], scap[32];
		// We don't support any capability now, so we're going to ignore.
		int ret = sscanf(recvbuffer, "[%s %s",
			       sname, scap);
		if (ret < 2) {
		    continue;
		}

		if (strcmp(sname, "TRIBALIA") ||
		    strcmp(scap, "CAPS")) {
		    continue;
		}

		// Send ours
		tinit.cli->SendTCP("[TRIBALIA CAPS ]\n");

		tinit.step = TCPInitStep::PlayerInfoRetrieve;			
	    }
	    break;
	case PlayerInfoRetrieve:
	    // Ask for the client name?
	    tinit.cli->SendTCP("[TRIBALIA PLAYERINFO?]\n");
	    tinit.step = TCPInitStep::PlayerInfoRetrieved;
	    break;
	case PlayerInfoRetrieved:
	    if (!tinit.cli->ReceiveTCP(recvbuffer, 192)) {
		continue;
	    }
	 
	    {    
		char sname[32], sinfo[32], splayername[64];
		int playerxp;
	    
		int ret = sscanf(recvbuffer, "[%s %s %s %d", sname,
				 sinfo, splayername, &playerxp);
		if (ret < 4)
		    continue;

		if (strcmp(sname, "TRIBALIA") ||
		    strcmp(sinfo, "PLAYERINFO")) {
		    continue;
		}

		tinit.cli->SetName(splayername);

		char ssendmsg[96];
		sprintf(ssendmsg, "[TRIBALIA PLAYERINFO %u]\n",
			tinit.cli->GetID());
		tinit.cli->SendTCP(ssendmsg);
	    }
	    
	    printf("%s is connected (%d loops, %d).\n", tinit.cli->GetName(),
		   tinit.iters, tinit.cli->GetStatus());
	    tinit.step = TCPInitStep::ClientReady;
	    if (tinit.cli->GetStatus() == CS_CONNECTING)
		tinit.cli->AdvanceStatus();
	    break;
	}
    
    }

    /* Remove closed or completed clients */
    std::remove_if(_initClients.begin(), _initClients.end(),
		   [](TCPInit c) {
		       return c.cli->IsClosed() ||
			   c.step == TCPInitStep::ClientReady; });
}

/* Check if we have clients to be initialized */
bool TCPConnectionInitiator::HasClient()
{
    return (_initClients.size() > 0);
}


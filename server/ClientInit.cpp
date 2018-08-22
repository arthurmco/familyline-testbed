/* Client initialization procedures
   Copyright (C) 2017 Arthur M
*/

#include "ClientInit.hpp"
#include <cstdio>

using namespace Familyline::Server;

TCPInit::TCPInit(Client* c)
{
    this->step = TCPInitStep::InitConnect;
    this->cli = c;
}

TCPConnectionInitiator::TCPConnectionInitiator(PlayerManager* pm)
    : _pm(pm)
{}

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
    memset((char*)recvbuffer, 0, 255);
    
    for (auto&& tinit : _initClients) {
	
	if (tinit.cli->GetStatus() >= CS_CONNECTED)
	    continue;

	tinit.iters++;
	switch (tinit.step) {
	case InitConnect:
	    if (!tinit.cli->GetQueue()->ReceiveTCP(recvbuffer, 128)) {
		continue;
	    }
	    
	    if (!strncmp(recvbuffer, "[TRIBALIA CONNECT OK]\n", 21)) {
		tinit.step = TCPInitStep::VersionQuery;
	    }
	    
	    break;
	case VersionQuery:
	    tinit.cli->GetQueue()->SendTCP("[TRIBALIA VERSION?]\n");
	    tinit.step = TCPInitStep::VersionQueried;
	    [[fallthrough]];
	case VersionQueried:
	    if (!tinit.cli->GetQueue()->ReceiveTCP(recvbuffer, 128)) {
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
			    tinit.cli->getName(), vmaj, vmin);
		    tinit.cli->GetQueue()->SendTCP("[TRIBALIA ERROR 001 Unsupported version]");
		    tinit.cli->Close();
		    continue;	    
		}

		tinit.step = TCPInitStep::CapabilityQuery;
	    }
	    break;
	case CapabilityQuery:
	    tinit.step = TCPInitStep::CapabilityQueried;
	    [[fallthrough]];
	case CapabilityQueried:
	    if (!tinit.cli->GetQueue()->ReceiveTCP(recvbuffer, 128)) {
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
		    strcmp(scap, "CAPS?]")) {
		    continue;
		}

		// Send ours
		tinit.cli->GetQueue()->SendTCP("[TRIBALIA CAPS ]\n");

		// Retrieve the client. Doesn't matter for now.
		tinit.cli->GetQueue()->ReceiveTCP(recvbuffer, 128);
		
		tinit.step = TCPInitStep::PlayerInfoRetrieve;			
	    }
	    break;
	case PlayerInfoRetrieve:
	    // Ask for the client name?
	    tinit.cli->GetQueue()->SendTCP("[TRIBALIA PLAYERINFO?]\n");
	    tinit.step = TCPInitStep::PlayerInfoRetrieved;
	    [[fallthrough]];
	case PlayerInfoRetrieved:
	    if (!tinit.cli->GetQueue()->ReceiveTCP(recvbuffer, 192)) {
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
		tinit.cli->GetQueue()->SendTCP(ssendmsg);
	    }
	    
	    printf("%s is connected (%d loops, %d).\n", tinit.cli->getName(),
		   tinit.iters, tinit.cli->GetStatus());
	    tinit.step = TCPInitStep::ClientReady;
	    if (tinit.cli->GetStatus() == CS_CONNECTING) {
		tinit.cli->AdvanceStatus();
		_pm->RegisterClient(tinit.cli);
	    }
	    break;
	}
    
    }

    /* Remove closed or completed clients */
    _initClients.erase(std::remove_if(
			   _initClients.begin(), _initClients.end(),
			   [](TCPInit c) {
			       return c.cli->IsClosed() ||
				   c.step == TCPInitStep::ClientReady ||
				   c.cli->GetStatus() >= CS_CONNECTED; }),
		       _initClients.end());
}

/* Check if we have clients to be initialized */
bool TCPConnectionInitiator::HasClient()
{
    return (_initClients.size() > 0);
}


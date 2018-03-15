#include "NetPlayerFilter.hpp"
#include <Log.hpp>

#include <stdio.h>
#include "NetServer.hpp"

using namespace Tribalia::Net;
using namespace Tribalia::Server;
/**
 * Get the corresponding message queue from the client 
 * 'client_id'
 */
ClientMessageQueue* NetPlayerFilter::GetPlayerQueue(unsigned client_id)
{
    try {
	return this->_cli_queues.at(client_id);
    } catch (const std::out_of_range& oor) {
	return nullptr;
    }
}


void NetPlayerFilter::SetServer(void* nse) { _nserver = nse; }


/**
 * Filter the messages
 */
void NetPlayerFilter::Filter()
{
    ((Net::Server*)this->_nserver)->GetMessages();
    bool has_message = true;
    while (has_message) {
	char* m = new char[512];
	auto mlen = this->_server_mq->PeekTCP(m, 512);
	if (mlen == 0) {
	    has_message = false;
	    break;
	}
	// TCP: Parse chat
	// (the unique client-dependent message)
	unsigned clientid = 0;
	char tr[16], ch[8];
	if (sscanf(m, "%9s %4s %d", tr, ch, &clientid) == 3) {
	    if (!strcmp(tr, "[TRIBALIA") && !strcmp(ch, "CHAT")) {
		auto pq = this->GetPlayerQueue(clientid);
		
		if (!pq) {
		    pq = new ClientMessageQueue(this->_server_mq->GetSocket(),
						this->_server_mq->GetAddress());
		    this->_cli_queues[clientid] = pq;
		    Log::GetLog()->Write("net-player-filter",
					 "Added client with ID %d", clientid);
		}

		printf("Chat Message: %s\n", &m[14]);
		this->_server_mq->ReceiveTCP(m, 512);
		pq->InjectMessageTCP(m, mlen+1);
	    }
	}

    }
    
}


NetPlayerFilter::~NetPlayerFilter()
{
    for (auto& cq : this->_cli_queues) {
	delete cq.second;
    }
}

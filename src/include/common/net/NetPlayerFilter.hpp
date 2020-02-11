/**
 * Filters the received messages from the client, putting them into its 
 * own queues
 *
 * Copyright (C) 2018 Arthur M
 */

#ifndef NETPLAYERFILTER_HPP
#define NETPLAYERFILTER_HPP

#include <common/NetMessageQueue.hpp>
#include <map>

namespace familyline::Net {
    
    class NetPlayerFilter
    {
    private:
	Server::NetMessageQueue* _server_mq = nullptr;

	std::map<unsigned /*clientid*/, Server::NetMessageQueue*> _cli_queues;
	void* _nserver = nullptr;
    
    public:
	NetPlayerFilter(Server::NetMessageQueue* server_mq)
	    : _server_mq(server_mq)
	    {}

	virtual ~NetPlayerFilter();

	/**
	 * Get the corresponding message queue from the client 
	 * 'client_id'
	 */
	Server::NetMessageQueue* GetPlayerQueue(unsigned client_id);

	/**
	 * Sets the netserver, the owner of 'server_mq'
	 */
	void SetServer(void* nse);
	
	
	/**
	 * Filter the messages
	 */
	void Filter();

	    

    };


}  // Familyline::Net


#endif /* NETPLAYERFILTER_HPP */

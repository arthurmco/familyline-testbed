/**
 * Filters the received messages from the client, putting them into its 
 * own queues
 *
 * Copyright (C) 2018 Arthur M
 */

#ifndef NETPLAYERFILTER_HPP
#define NETPLAYERFILTER_HPP

#include <ClientMessageQueue.hpp>
#include <map>

namespace Tribalia::Net {
    class NetPlayerFilter
    {
    private:
	Server::ClientMessageQueue* _server_mq = nullptr;

	std::map<unsigned /*clientid*/, Server::ClientMessageQueue*> _cli_queues;
    
    public:
	NetPlayerFilter(Server::ClientMessageQueue* server_mq)
	    : _server_mq(server_mq)
	    {}
	    
	virtual ~NetPlayerFilter();

	/**
	 * Get the corresponding message queue from the client 
	 * 'client_id'
	 */
	Server::ClientMessageQueue* GetPlayerQueue(unsigned client_id);

	/**
	 * Filter the messages
	 */
	void Filter();

	    

    };


}  // Tribalia::Net


#endif /* NETPLAYERFILTER_HPP */

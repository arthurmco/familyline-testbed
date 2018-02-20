/**
 * Server administration control class
 *
 * The interfaces will control the server interating with code here
 *
 * Copyright (C) 2018 Arthur M
 */

#ifndef ADMINCOMMANDPARSER_HPP
#define ADMINCOMMANDPARSER_HPP

#include "ChatLogger.hpp"
#include <ServerPlayerManager.hpp>

namespace Tribalia::Server {
    constexpr int SERVER_PORT = 12100;

/**
 * Information about the connected server interface 
 */
struct ServerInterface {
    socket_t clisock;
    unsigned id;
};

/**
 * The server is controlled via the Tribalia Server Admin Protocol
 * (No abbreviations to avoid lawsuits from strong software german companies)
 *
 * This class manages this command parsing and interpretation
 *
 * TODO: Maybe support Unix sockets too?
 */
class AdminCommandParser {
private:
    socket_t sock = -1; // The socket

    std::list<ServerInterface> interface_lists;
    
    PlayerManager* _spm;
    ChatLogger* _lm;

    /**
     * Process the 'player list' request
     */
    bool ProcessPlayerListRequest(socket_t clisocket);

    /**
     * Process the 'chat list' request
     * @param timestamp The starting timestamp to download the messages, only
     *                  messages newer than the timestamp will be sent to the
     *                  interface
     */
    bool ProcessChatListRequest(socket_t clisocket, unsigned long timestamp);

    /**
     * Send a chat message from the interface
     * Receive it from the admin socket and send it to the clients
     */
    bool SendChat(socket_t clisocket);
    
public:
    AdminCommandParser(PlayerManager* spm, ChatLogger* lm)
	: _spm(spm), _lm{lm}
	{}

    /**
     * Start listening for connections from the interface
     */
    bool Listen();


    /**
     * Main loop. It receives requests from interfaces, processes them and 
     * return the results
     *
     * It also manages the connections to the same clients.
     */
    void ProcessRequests();

   
    ~AdminCommandParser();
};


}

#endif /* ADMINCOMMANDPARSER_HPP */

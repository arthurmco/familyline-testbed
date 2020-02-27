/***
	Chat management (send and retrieval) in Familyline

	Copyright (C) 2017, 2018 Arthur M
***/

#include <cstdio>
#include <algorithm>

#include <chrono>

#include "Client.hpp"

#ifndef CHATMANAGER_HPP
#define CHATMANAGER_HPP

namespace familyline::Server {

	enum ChatDestiny {
		ToClient,
		ToTeam,
		ToAll,
		ToInvalid
	};

	/* Basic structure of the chat message */
	struct ChatMessage {
		std::chrono::seconds time; // unix timestamp of the message arrival
		ChatDestiny destiny;
		int destiny_id; // the ID of the destination, depends on the 'destiny' val
		char* message;
		Client* sender; // the sender, null if the server send it.
	};

	class ChatManager {
	private:

	public:
		/* Send a chat message to the specified client */
		void Send(Client* c, Client* sender, const char* message);

		/* Send a message made for a clients connected.
		   This receives only a client because these functions only operate in
		   a client a time */
		void SendAll(Client* c, Client* sender, const char* message);

		/* Checks if a message arrived from client 'c'.
		   Returns nullptr if no message, otherwise return the message */
		ChatMessage* CheckMessage(Client* c);

	};

}

#endif /* CHATMANAGER_HPP */
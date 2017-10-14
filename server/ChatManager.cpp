#include "ChatManager.hpp"

using namespace Tribalia::Server;

/* Send a chat message to the specified client */
void  ChatManager::Send(Client* c, const char* message)
{
    size_t mlen = strlen(message);
    char* msg = new char[mlen + 64];

    sprintf(msg, "[TRIBALIA CHAT 0 player:%u %zu %s]\n",
	    c->GetID(), mlen, message);
    c->SendTCP(msg);
    
    delete[] msg;
}

/* Send a message made for a clients connected.
   This receives only a client because these functions only operate in
   a client a time */
void ChatManager::SendAll(Client* c, const char* message)
{
    size_t mlen = strlen(message);
    char* msg = new char[mlen + 64];

    sprintf(msg, "[TRIBALIA CHAT 0 all %zu %s]\n",
	    mlen, message);
    c->SendTCP(msg);
    
    delete[] msg;
}

/* Checks if a message arrived from client 'c'.
   Returns nullptr if no message, otherwise return the message content */
ChatMessage* ChatManager::CheckMessage(Client* c)
{
    char expect[64];
    sprintf(expect, "[TRIBALIA CHAT %u", c->GetID());

    if (!c->PeekTCP(expect, strlen(expect))) {
	return nullptr;
    }

    char msg[385];
    c->ReceiveTCP(msg, 384);

    char sname[32], schat[8], sdestiny[24], smessage[255];
    size_t mlen;
    int senderid;

    int msgpos = 0;
    int ret = sscanf(msg, "[%s %s %d %s %zu%n",
		     sname, schat, &senderid, sdestiny, &mlen, &msgpos);
    if (ret < 5) {
	fprintf(stderr, "received garbage chat message from %s\n",
		c->GetName());
	return nullptr;
    }

    strncpy(smessage, &msg[msgpos+1], std::min(mlen, size_t(255)));
    int did = 0;

    // Determine destination
    ChatDestiny d = ChatDestiny::ToInvalid;
    if (!strncmp(sdestiny, "player", 6)) {
	d = ChatDestiny::ToClient;
	char cplayer[16];
	sscanf(sdestiny, "%s:%d", cplayer, &did);
    } else if (!strcmp(sdestiny, "team")) {
	d = ChatDestiny::ToTeam;
    } else if (!strcmp(sdestiny, "all")) {
	d = ChatDestiny::ToAll;
    }

    if (d == ChatDestiny::ToInvalid) {
	fprintf(stderr, "received chat message from %s with invalid destination\n",
		c->GetName());
	return nullptr;
    }

    smessage[std::min(mlen, size_t(254))] = 0;

    ChatMessage* cm = new ChatMessage{};
    cm->destiny = d;
    cm->destiny_id = did;
    cm->message = smessage;
    return cm;
      
}

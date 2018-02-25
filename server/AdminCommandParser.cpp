#include "AdminCommandParser.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstdio>
#include <cstdlib> //exit()
#include <cstring> //memset(), strncpy()
#include <errno.h> //perror()
#include <unistd.h> //close()
#include <poll.h> //poll()

#include <list>
#include <algorithm> //std::min()
#include <chrono>

using namespace Tribalia::Server;

#include <Log.hpp>
#include "ChatManager.hpp"

/**
 * Start listening for connections from the interface
 */
bool AdminCommandParser::Listen()
{
    this->sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if ( bind(this->sock, (sockaddr*)&addr, sizeof(addr)) ) {
	// Bind failed
	Log::GetLog()->Fatal("admin-command",
			     "Bind failed: %s", strerror(errno));
	return false;
    }

    if ( listen(this->sock, 4) ) {
	// Listen failed
	Log::GetLog()->Fatal("admin-command",
			     "Listen failed: %s", strerror(errno));
	return false;
    }

    Log::GetLog()->Write("admin-command",
			 "Socket listening at 0.0.0.0:%d", SERVER_PORT);
    return true;
}


/**
 * Process the 'player list' request
 */
bool AdminCommandParser::ProcessPlayerListRequest(socket_t clisocket)
{
    char msg[256];
    auto s = recv(clisocket, msg, 255, MSG_PEEK);
    if (s <= 0) {
	// Error or disconnect
	return false;
    }

    msg[s] = '\0';
    if (strcmp(msg, "[TRIBALIA REQUEST PLAYERS ]\n"))
	return false; // Not the same message

    s = recv(clisocket, msg, s, 0); // remove message from queue

    char smsg[2048];
    sprintf(smsg, "[TRIBALIA RESPONSE PLAYERS %zu ",
	    this->_spm->GetPlayers().size());
    for (const auto& pl : this->_spm->GetPlayers()) {
	char pmsg[128];
	snprintf(pmsg, 128, "%d %s %d %s %s ",
		 pl.cli->GetID(), pl.cli->GetName(), 0,
		 (pl.cli->GetStatus() >= CS_CONNECTED) ? "connected" : "connecting",
		 "192.168.0.1");
	strcat(smsg, pmsg);
    }
    strcat(smsg, "]\n");
    
    send(clisocket, smsg, strlen(smsg), 0);
    return true;
}

/**
 * Process the 'chat list' request
 * @param timestamp The starting timestamp to download the messages, only
 *                  messages newer than the timestamp will be sent to the
 *                  interface
 */
bool AdminCommandParser::ProcessChatListRequest(socket_t clisocket)
{
    char msg[128];
    
    auto s = recv(clisocket, msg, 32, MSG_PEEK);
    if (s <= 0) {
	// Error or disconnect
	return false;
    }

    msg[s] = '\0';
    if (strncmp(msg, "[TRIBALIA REQUEST CHAT", 22))
	return false; // Not the same message

    s = recv(clisocket, msg, 127, 0);

    unsigned long long since_epoch = 0;
    char mtr[16], r[16], ch[16];
    sscanf(msg, "%s %s %s %lld", mtr, r, ch, &since_epoch);

    Log::GetLog()->Write("admin-command-parser", "Requested messages since epoch %lld", since_epoch);
    
    auto messages = this->_lm->GetMessagesNewerThan(since_epoch);
    Log::GetLog()->Write("admin-command-parser", "%zu messages returned", messages.size());
    
    if (messages.size() == 0) {
	const char* mchat = "[TRIBALIA CHAT NULL]";
	send(clisocket, mchat, strlen(mchat), 0);
	return true;
    }

    for (auto m : messages) {
	char mchat[strlen(m->message) + 128];
	sprintf(mchat, "[TRIBALIA CHAT %ld %d %s %s %zu %s ]",
		m->time.count(),
		(m->sender) ? m->sender->GetID() : 0,
		(m->sender) ? m->sender->GetName() : "admin",
		"all", strlen(m->message), m->message);
	
	send(clisocket, mchat, strlen(mchat), 0);
    }

    return true;
}

/**
 * Send a chat message from the interface
 * Receive it from the admin socket and send it to the clients
 */
bool AdminCommandParser::SendChat(socket_t clisocket)
{
    char msg[512];
    auto s = recv(clisocket, msg, 32, MSG_PEEK);
    if (s <= 0) {
	// Error or disconnect
	return false;
    }

    msg[s] = '\0';
    if (strncmp(msg, "[TRIBALIA SEND-CHAT", 19))
	return false; // Not the same message

    s = recv(clisocket, msg, 511, 0);
    msg[s] = '\0';

    char mtr[10], mse[10], desttype[16];
    int msgsize = 0;
    int msgoffset = 0;

    sscanf(msg, "%s %s %s %d %n", mtr, mse, desttype, &msgsize,
	&msgoffset);

    // prevents overflows with wrong sizes
    msgsize = std::min(unsigned(strlen(msg) - msgoffset), unsigned(msgsize));

    char* msgcontent = new char[msgsize];

    strncpy(msgcontent, &msg[msgoffset], msgsize);
    msgcontent[msgsize] = '\0';

    // TODO: set message destiny correctly
    ChatMessage* m = new ChatMessage();
    m->message = msgcontent;
    m->destiny = ChatDestiny::ToAll;
    m->time = std::chrono::duration_cast<std::chrono::seconds>
	(std::chrono::system_clock::now().time_since_epoch());
    m->sender = nullptr;

    _lm->Push(m);
    return true;
}

/**
 * Main loop. It receives requests from interfaces, processes them and 
 * return the results
 *
 * It also manages the connections to the same clients.
 */
void AdminCommandParser::ProcessRequests()
{
    unsigned i = 0;

    // Poll the interfaces plus the server listen socket for any incoming data
    auto fdslen = interface_lists.size() + 1;
    struct pollfd fds[fdslen];

    for (auto& ilist : this->interface_lists) {
	fds[i].fd = ilist.clisock;
	fds[i].events = POLLIN;
	i++;
    }

    fds[i].fd = this->sock;
    fds[i].events = POLLIN;

    auto r = poll(fds, fdslen, 10);
    if (r < 0) {
	// Error occurred
	Log::GetLog()->Warning("admin-command",
			       "poll failed: %s", strerror(errno));
	return;
    } else if ( r == 0)  {
	// No response for any socket
	return;
    }

    /// Some socket answered. What one?


    // Some helping lambdas
    auto fnGetInterfaceID = [this] (unsigned idx) {

	auto it = this->interface_lists.begin();
	std::advance(it, idx);

	return it->id;
    };
    
    // Remove an item from the interface list given its index
    auto fnErase = [this] (unsigned idx) {

	auto it = this->interface_lists.begin();
	std::advance(it, idx);

	this->interface_lists.erase(it);
    };
    
    i = 0;
    const auto serversock = fdslen-1; // The server will always be the last one.
    for (i = 0; i < fdslen-1; i++) {
	if (fds[i].revents & POLLHUP) {
	    // Interface disconnected
	    Log::GetLog()->Write("admin-command",
				"connection from interface %d closed by remote host\n",
				fnGetInterfaceID(i));

	    fnErase(i);
	    continue;
	}

	if (fds[i].revents & POLLERR) {
	    // Some error happened
	    fprintf(stderr, "Connection error on interface %d\n",
		    fnGetInterfaceID(i));
	    fnErase(i);
	    continue;
	}

	if (fds[i].revents & POLLIN) {
	    char data[256];
	    auto siface = fnGetInterfaceID(i);

	    if (this->ProcessPlayerListRequest(fds[i].fd))
		continue;

	    if (this->ProcessChatListRequest(fds[i].fd))
		continue;

	    if (this->SendChat(fds[i].fd))
		continue;
		
	    
	    fprintf(stderr, "Data received on interface %d\n",
		    siface);

	    auto dlen = read(fds[i].fd, (void*)data, 255);
	    if (dlen < 0) {
		fprintf(stderr, "Read error on interface %d\n",
			siface);
		return;
	    } else if (dlen == 0) {
		// Interface disconnected
		Log::GetLog()->Write("admin-command",
				    "connection from interface %d closed by remote host\n",
				    fnGetInterfaceID(i));


		fnErase(i);
		continue;
	    }

	    data[dlen] = 0;
	    fprintf(stderr, "Received '%s' (%zu) from %d \n", data,
		    dlen, siface);
	    
	}
	
    }

    if (fds[serversock].revents & POLLIN) {
	// Connection incoming

	struct sockaddr_in raddr;
	socklen_t raddrlen = 0;
	auto csock = accept(this->sock, (struct sockaddr*)&raddr, &raddrlen);

	if (csock < 0) {
	    fprintf(stderr, "accept() failed: %s\n", strerror(errno));
	    exit(1);		
	}

	ServerInterface si;
	si.clisock = csock;
	si.id = csock | (serversock << 8); // some simple calculations, ID doesn't matter
	char msg[128];
	sprintf(msg, "[TRIBALIA INTERFACE %d () ]\n", si.id);
	write(csock, msg, strlen(msg));

	bool continue_con = false;

	while (!continue_con) {
	    // TODO: Move this write() out of here.
	    // This can cause denial of service
	    auto s = read(csock, msg, 128);
	    if (s == 0) {
		// Connection ended prematurely
		return;
	    } else if (s < 0) {
		fprintf(stderr, "Error in handshake with %d: %s\n",
			si.id, strerror(errno));
		return;
	    }

	    msg[s] = '\0';

	    if (!strcmp("[TRIBALIA INTERFACE OK ]\n", msg))
		continue_con = true;
	}
	
	

	char ipstr[INET_ADDRSTRLEN];
	const char* ipaddr = inet_ntop(AF_INET, (void*) &(raddr.sin_addr),
				       ipstr, raddrlen);
	Log::GetLog()->Write("admin-command",
			    "New interface detected, ip %s, id %d",
			    ipaddr, si.id);
	
	
	interface_lists.push_back(si);
    }
}


AdminCommandParser::~AdminCommandParser()
{
    Log::GetLog()->Write("admin-command", "Closing listen socket");
    if (this->sock >= 0)
	shutdown(this->sock, SHUT_RDWR);

    close(this->sock);
}


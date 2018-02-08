#include "ServerManager.hpp"
#include <Log.hpp>

using namespace Tribalia;
using namespace Tribalia::Server;

/* Starts a server manager in the specified port */
ServerManager::ServerManager(int port)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0) {
	char* syserr = strerror(errno);
	char excerr[strlen(syserr) + 48];
	sprintf(excerr, "Error while binding socket: %s", syserr);
	close(sockfd);
	throw ServerManagerError{excerr};
    }
    

    if (listen(sockfd, 12) < 0) {
	char* syserr = strerror(errno);
	char excerr[strlen(syserr) + 48];
	sprintf(excerr, "Error while listening socket: %s", syserr);
	close(sockfd);
	throw ServerManagerError{excerr};
    }

    started = true;
}

/* Set server network address, in form of 
   ip/netmask (ex: 192.168.1.0/255)
*/
void ServerManager::SetNetworkAddress(char* naddr)
{
    char* saddr = strdup(naddr);
    char* snetmask = strchr(saddr, '/');
    auto addrlen = size_t(snetmask - saddr);
    snetmask++;

    int netmask = std::stoi(snetmask);
    saddr[addrlen] = 0;
    
    printf("Changed server network to %s/%d\n", saddr, netmask);

    inet_aton(saddr, &(this->serv_net_addr_base));
    serv_netmask = short(netmask & 0xffff);

    free(saddr);   
}

/* Check if address is from the same network as the server
   Return if is not
*/
bool ServerManager::CheckAddress(struct in_addr* addr)
{
    if (addr->s_addr == htonl(INADDR_LOOPBACK))
	return true; // loopback is always valid

    if (serv_net_addr_base.s_addr == INADDR_ANY)
	return true; // any address, always valid

    auto cli_saddr = htonl(addr->s_addr);
    auto serv_saddr = htonl(this->serv_net_addr_base.s_addr);
    auto mask = (1 << this->serv_netmask) - 1;

    mask = mask << ((8*sizeof(cli_saddr)) - this->serv_netmask);

    return ((cli_saddr & mask) == (serv_saddr & mask));
}


bool blocked = true;

/* Retrieve a client, if available
   If blocks = true, blocks until next client is available.
   If is false, then return null if no client available
*/
Client* ServerManager::RetrieveClient(bool blocks)
{
    if (blocked != blocks) {
	/* Set socket to nonblocking if set to nonblock, or to
	 blocking if not*/
	int flags = fcntl(this->sockfd, F_GETFL, 0);
	if (flags < 0) {
	    return nullptr;
	}
	flags = (blocks ? (flags & O_NONBLOCK) : (flags | O_NONBLOCK));
	fcntl(this->sockfd, F_SETFL, flags);
    }

    blocked = blocks;
do_retrieve_client:

    
    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(struct sockaddr_in));

    socklen_t cliaddr_len = sizeof(struct sockaddr_in);
    int clisockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &cliaddr_len);

    if (clisockfd < 0) {
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
	    if (!blocks) {
		/* This sleep is merely to force the processor to change
		   tasks, and not make our process 100% of CPU with no need
		*/
		usleep(100);
		return nullptr;
	    }
	}
	
	char* syserr = strerror(errno);
	char excerr[strlen(syserr) + 48];
	sprintf(excerr, "Error while accepting socket: %s", syserr);
	throw ServerManagerError{excerr};
    }

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, (void*)&(cliaddr.sin_addr), ipstr, INET_ADDRSTRLEN);
    if (!this->CheckAddress(&(cliaddr.sin_addr))) {
	printf("Socket refused: not in the same network "
	       "(fd %d), address %s\n", clisockfd, ipstr);
	return nullptr;
    }   
    
    printf("Socket accepted (fd %d), address %s\n", clisockfd, ipstr);

    char* str = new char[1492];
    auto sread = read(clisockfd, (void*)str, 1490);

    /* Remove nonblocking from the client.
       We use poll(), no need for this */
    int flags = fcntl(clisockfd, F_GETFL, 0);
    fcntl(this->sockfd, F_SETFL, flags ^ O_NONBLOCK);
    
    if (sread < 0) {	
	shutdown(clisockfd, 2);
	close(clisockfd);
	char* syserr = strerror(errno);
	char excerr[strlen(syserr) + 48];
	sprintf(excerr, "Error while reading from socket: %s", syserr);
	throw ServerManagerError{excerr};
    }

    if (sread == 0) {
	if (blocks)
	    goto do_retrieve_client;
	
	return nullptr;
    }

    printf("%zu bytes read, content follows:\n", (size_t)sread);
    str[sread+1] = 0;
    printf("%s\n===========================", str);

    std::string sstr{str};
    /* Test html service */
    if (sstr.find("[TRIBALIA CONNECT]\n") != std::string::npos ) {
	char connback[48+INET_ADDRSTRLEN];

	sprintf(connback, "[TRIBALIA CONNECT %s]\n", ipstr);
	if (write(clisockfd, connback, strlen(connback)) > 0) {
		
	    std::shared_ptr<Client> c =
		std::make_shared<Client>(clisockfd, cliaddr.sin_addr);
	    Log::GetLog()->InfoWrite("server-manager",
				     "Client added (fd %d), address %s\n",
				     clisockfd, ipstr);    
	    clients.push_back(c);
	    return c.get();
	}
    }

    shutdown(clisockfd, 2);
    close(clisockfd);
    
    if (blocks)
	goto do_retrieve_client;
    
    return nullptr;

}


/* Poll for TCP messages and redirect them to the appropriate client */
void ServerManager::RetrieveTCPMessages() {
    std::remove_if(clients.begin(), clients.end(), [](std::shared_ptr<Client> c)
		   {  return !c || !c.get() || c->IsClosed(); });
    
    auto client_qt = clients.size();
    if (client_qt == 0)
	return;

    /* Build the pollfds */
    struct pollfd pfds[client_qt];
    memset(pfds, 0, sizeof(pfds));

    const struct timespec timeout = {0, 30000000}; // 30 ms timeout
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);

    size_t client_poll_qt = 0;
    for (size_t i = 0; i < client_qt; i++) {
	if (clients[i]) {
	    pfds[i].fd = clients[i]->GetQueue()->GetSocket();
	    pfds[i].events = POLLIN;  // poll for read and conn end
	    client_poll_qt++;
	}
    }

    auto res = ppoll(pfds, client_poll_qt, &timeout, &sigset);
    if (res > 0) {
	// some descriptors are ready
//	printf("Retrieving message from sockets: ");
    
	char readbuf[1536];
	for (size_t i = 0; i < client_poll_qt; i++) {
	    if (pfds[i].revents) {
		if (pfds[i].revents & POLLIN) {
		    auto& cli = clients[i];

		    auto readnum = read(cli->GetQueue()->GetSocket(), (void*)readbuf, 1535);
		    if (readnum == 0) {
			/* Return 0 in reads means remote disconnection  */
			Log::GetLog()->Write("server-manager",
					     "Client %d (%s) disconnected",
					     cli->GetID(), cli->GetName());
			
			cli->Close();
			clients.erase(clients.begin()+i);
			continue;
		    }
		    
		    if (cli->CheckHeaders() &&
			strncmp(readbuf, "[TRIBALIA", 9) != 0) {
			printf("error: bad header from %d\n", cli->GetQueue()->GetSocket());
			cli->Close();
			//TODO: count number of errors before closing
			continue;
		    }
		    
		    cli->GetQueue()->InjectMessageTCP(readbuf, size_t(readnum));
		    memset(readbuf, 0, readnum);
		}

		if (pfds[i].revents & POLLHUP) {
		    auto& cli = clients[i];
		    Log::GetLog()->Write("server-manager",
					     "Client %d (%s) disconnected (POLLHUP)",
					     cli->GetID(), cli->GetName());

		    cli->Close();
		    clients.erase(clients.begin()+i);
		}


	    }
	}
	

    } else if (res == 0) {
	// none ready
	return;
    } else {
	// error
	char* syserr = strerror(errno);
	char excerr[strlen(syserr) + 48];
	sprintf(excerr, "Error while polling socket: %s", syserr);
	throw ServerManagerError{excerr};
    }

}

ServerManager::~ServerManager()
{
    if (started) {
	shutdown(sockfd, 2);
	close(sockfd);
    }
}

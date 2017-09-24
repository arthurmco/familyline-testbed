#include "ServerManager.hpp"

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
	flags = blocks ? (flags &= O_NONBLOCK) : (flags |= O_NONBLOCK);
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
	    if (!blocks)
		return nullptr;
	}
	
	char* syserr = strerror(errno);
	char excerr[strlen(syserr) + 48];
	sprintf(excerr, "Error while accepting socket: %s", syserr);
	throw ServerManagerError{excerr};
    }

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, (void*)&(cliaddr.sin_addr), ipstr, INET_ADDRSTRLEN);
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
    if (sstr.find("GET / HTTP/1.1") != std::string::npos ||
	sstr.find("GET / HTTP/1.0") != std::string::npos) {

	printf("Got valid answer\n");

	char* stranswer = new char[600];

	sprintf(stranswer, "<html>"
	    "<head><title>Tribalia Test Server</title>"
		"<meta name=\"viewport\" content=\"width=device-width, "
		"initial-scale=1\"></head>"
		"<body>This is a test message from the server. <br/> "
		"You are <b>%s</b></body>", ipstr);

	printf("Serving %zu bytes of content\n", strlen(stranswer));
	char* strmsg = new char[strlen(stranswer)+300];

	char strdate[64];

	time_t rawtime;
	struct tm* timeinfo;
	
	time(&rawtime);
	timeinfo = gmtime(&rawtime);

	strftime(strdate, 64, "%a, %d %h %Y %T GMT", timeinfo);
	
	sprintf(strmsg, ""
	    "HTTP/1.1 200 OK\r\n"
	    "Date: %s\r\n"
	    "Status: 200 OK\r\n"
	    "Content-Type: text/html; charset=utf-8\r\n"
	    "Content-Length: %zu\r\n" 
	    "Server: tribalia-server\r\n"
		"\r\n\r\n%s", strdate, strlen(stranswer), stranswer);
		
	write(clisockfd, (void*)strmsg, strlen(strmsg));
    }

    std::shared_ptr<Client> c = std::make_shared<Client>(clisockfd,
							 cliaddr.sin_addr);
    printf("Client added (fd %d), address %s\n", clisockfd, ipstr);    
    clients.push_back(c);
    return c.get();
}


/* Poll for messages and redirect them to the appropriate client */
void ServerManager::RetrieveMessages() {
    std::remove_if(clients.begin(), clients.end(), [](std::shared_ptr<Client> c)
		   {  return c->IsClosed(); });
    
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
    
    for (size_t i = 0; i < client_qt; i++) {
	pfds[i].fd = clients[i]->GetSocket();
	pfds[i].events = POLLIN | POLLHUP;  // poll for read and conn end
    }

    auto res = ppoll(pfds, client_qt, &timeout, &sigset);
    if (res > 0) {
	// some descriptors are ready
//	printf("Retrieving message from sockets: ");
    
	char readbuf[1536];
	for (size_t i = 0; i < client_qt; i++) {
	    if (pfds[i].revents) {
		if (pfds[i].revents & POLLIN) {
		    auto& cli = clients[i];

		    auto readnum = read(cli->GetSocket(), (void*)readbuf, 1535);
		    if (readnum == 0)
			continue;
		    
		    printf("%d (%zd)", cli->GetSocket(), readnum);
		    
		    cli->InjectMessage(readbuf, size_t(readnum));
		    memset(readbuf, 0, readnum);
		}

		if (pfds[i].revents & POLLHUP) {
		    auto& cli = clients[i];
		    printf(" %d (disconnected)", cli->GetSocket());

		    clients[i]->Close();
		}

		printf("\n");

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

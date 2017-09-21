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
    

    if (listen(sockfd, 0) < 0) {
	char* syserr = strerror(errno);
	char excerr[strlen(syserr) + 48];
	sprintf(excerr, "Error while listening socket: %s", syserr);
	close(sockfd);
	throw ServerManagerError{excerr};
    }

    started = true;
}
	
/* Retrieve a client, if available
   If blocks = true, blocks until next client is available.
   If is false, then return null if no client available
*/
Client* ServerManager::RetrieveClient(bool blocks)
{
    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(struct sockaddr_in));

    socklen_t cliaddr_len = sizeof(struct sockaddr_in);
    int clisockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &cliaddr_len);

    if (clisockfd < 0) {
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
    if (sread < 0) {
	shutdown(clisockfd, 2);
	close(clisockfd);
	char* syserr = strerror(errno);
	char excerr[strlen(syserr) + 48];
	sprintf(excerr, "Error while reading from socket: %s", syserr);
	throw ServerManagerError{excerr};
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
	    "<head><title>Tribalia Test Server</title></head>"
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

ServerManager::~ServerManager()
{
    if (started) {
	shutdown(sockfd, 2);
	close(sockfd);
    }
}

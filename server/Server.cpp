/***
 	Tribalia server entry point	

	Copyright (C) 2016 Arthur M
***/

#include "../src/EnviroDefs.h"

#include "ServerManager.hpp"
#include <list>

#include <signal.h>

using namespace Tribalia::Server;

volatile bool continue_main = true;

static void test_http_respond(Client* c, const char* msg) {
    std::string sstr{msg};
    /* Test html service */
    if (sstr.find("GET / HTTP/1.1") != std::string::npos ||
	sstr.find("GET / HTTP/1.0") != std::string::npos) {

	printf("Got valid answer\n");

	char ipstr[INET_ADDRSTRLEN];
	auto addr = c->GetAddress();
	inet_ntop(AF_INET, (void*)&addr, ipstr, INET_ADDRSTRLEN);


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
		
	c->Send(strmsg);
    }
}

int main(int argc, char const* argv[]) 
{
    struct sigaction oact;
    oact.sa_handler = [](int sig){ (void)sig; continue_main = false; };
    sigemptyset(&oact.sa_mask);
    oact.sa_flags = SA_RESTART;
    sigaction(SIGINT, &oact, nullptr);
    
    
    printf("Tribalia Server " VERSION "\n");
    printf("Copyright (C) 2016 Arthur M\n ");
    printf("Listening on port 12000\n");
    puts("\n");

    ServerManager* sm = nullptr;
    std::list<Client*> clis;
	
    try {
	sm = new ServerManager{};
	while (continue_main) {
	    sm->RetrieveMessages();
	    Client* c = sm->RetrieveClient(false);
	    if (c) {
		clis.push_back(c);
	    }

	    for (auto& cli : clis) {
		char m[2049];
		memset(m, 0, 2048);
		if (cli->Receive(m, 2048)) {
		    printf("From %d: "
			   "\033[3;37m\n%s\033[0m\n",
			   cli->GetSocket(), m);
		    test_http_respond(cli, m);
		}
		    
	    }
	    
	    
	    usleep(300);
	}
	
	delete sm;	    
    } catch (ServerManagerError& e) {
	fprintf(stderr, "Error: %s\n", e.what());
	return 1;
    }


    printf("Exit");
    return 0;
}	

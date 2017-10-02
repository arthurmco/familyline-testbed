/***
 	Tribalia server entry point	

	Copyright (C) 2016 Arthur M
***/

#include "../src/EnviroDefs.h"

#include "ServerManager.hpp"
#include <list>

#include <signal.h>
#include <ctime>
#include <string>
#include <cstdio>

using namespace Tribalia::Server;

volatile bool continue_main = true;

int main(int argc, char const* argv[]) 
{
    struct sigaction oact;
    oact.sa_handler = [](int sig){ (void)sig; continue_main = false; };
    sigemptyset(&oact.sa_mask);
    oact.sa_flags = SA_RESTART;
    sigaction(SIGINT, &oact, nullptr);  
    
    printf("Tribalia Server " VERSION "\n");
    printf("Copyright (C) 2017 Arthur M\n");
    printf("Listening on port 12000\n");
    puts("\n");

    ServerManager* sm = nullptr;
    std::list<Client*> clis;

    TCPConnectionInitiator tci;
	
    try {
	sm = new ServerManager{};
	
	while (continue_main) {
	    sm->RetrieveTCPMessages();
	    Client* c = sm->RetrieveClient(false);
	    if (c) {
		clis.push_back(c);
		tci.AddClient(c);
	    }

	    if (tci.HasClient())
		tci.Process();

	    for (auto& cli : clis) {
		if (cli->IsClosed()) {
		    continue;
		}

		if (cli->GetStatus() > CS_CONNECTED) {
		    char m[2049];
		
		    memset(m, 0, 2048);
		    if (cli->ReceiveTCP(m, 2048)) {
		    
			printf("From %d: "
			       "\033[3;37m\n%s\033[0m\n",
			       cli->GetSocket(), m);
		    }
		}
		    
	    }
	    
	    
	    usleep(100);
	}
	
	delete sm;	    
    } catch (ServerManagerError& e) {
	fprintf(stderr, "Error: %s\n", e.what());
	return 1;
    }


    printf("Exit");
    return 0;
}	

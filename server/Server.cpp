/***
 	Tribalia server entry point	

	Copyright (C) 2016 Arthur M
***/

#include "../src/EnviroDefs.h"

#include "ServerManager.hpp"

#include <signal.h>

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
    printf("Copyright (C) 2016 Arthur M\n ");
    printf("Listening on port 12000\n");
    puts("\n");

    ServerManager* sm = nullptr;
	
    try {
	sm = new ServerManager{};
	while (continue_main) {
	    sm->RetrieveMessages();
	    sm->RetrieveClient(false);
	    usleep(300);
	}
	
	delete sm;	    
    } catch (ServerManagerError& e) {
	fprintf(stderr, "Error: %s", e.what());
	return 1;
    }


    printf("Exit");
    return 0;
}	

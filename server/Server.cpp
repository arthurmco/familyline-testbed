/***
 	Tribalia server entry point	

	Copyright (C) 2016 Arthur M
***/

#include "../src/EnviroDefs.h"

#include "ServerManager.hpp"

using namespace Tribalia::Server;

int main(int argc, char const* argv[]) 
{

	printf("Tribalia Server " VERSION "\n");
	printf("Copyright (C) 2016 Arthur M\n ");
	printf("Listening on port 12000\n");
	puts("\n");

	ServerManager* sm = nullptr;
	
	try {
	    sm = new ServerManager{};
	    sm->RetrieveClient();
	    delete sm;	    
	} catch (ServerManagerError& e) {
	    fprintf(stderr, "Error: %s", e.what());
	    return 1;
	}


	printf("Exit");
	return 0;
}	

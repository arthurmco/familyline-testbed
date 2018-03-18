/***
 	Tribalia server entry point	

	Copyright (C) 2016, 2017 Arthur M
***/

#include "../src/EnviroDefs.h"

#include "ServerManager.hpp"
#include "ChatManager.hpp"
#include "ClientInit.hpp"
#include "ServerPlayerManager.hpp"
#include "AdminCommandParser.hpp"

#include <list>
#include <Log.hpp>

#include <signal.h>
#include <ctime>
#include <string>
#include <cstdio>

using namespace Tribalia;
using namespace Tribalia::Server;

volatile bool continue_main = true;

int main(int argc, char const* argv[]) 
{
    (void)argc;
    (void)argv;
    
    Log::GetLog()->SetFile(stderr);
    
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
    ChatManager* chm = nullptr;
    PlayerManager* pm = new PlayerManager();
    ChatLogger cl;
    
    std::list<Client*> clis;

    TCPConnectionInitiator tci(pm);

    try {
	sm = new ServerManager{};
	chm = new ChatManager{};
	AdminCommandParser acp(pm, &cl);
	if (!acp.Listen())
	    throw ServerManagerError("Error while listen()ing to the admin command parser");

	bool game_starting = false;
	
	while (continue_main) {
	    sm->RetrieveTCPMessages();
	    acp.ProcessRequests();
	    
	    Client* c = sm->RetrieveClient(false);

	    if (c) {
		clis.push_back(c);
		tci.AddClient(c);
	    }

	    if (tci.HasClient())
		tci.Process();

	    pm->Process();

	    for (auto cli : clis) {
		if (cli->IsClosed()) {
		    continue;
		}	
		
		if (cli->GetStatus() >= CS_CONNECTED) {
		    char m[2049];

		    // Check if client is ready or not anymore
		    if (cli->GetQueue()->PeekTCP(m, 40)) {
			if (!strncmp(m, "[TRIBALIA GAME READY]", 21) &&
			    !cli->IsReady()) {

			    cli->GetQueue()->ReceiveTCP(m, 40);
			    printf("\033[1mClient %s is ready\033[0m\n",
				   cli->GetName());
			    cli->SetReady();
			    continue;
			}

			if ((!strncmp(m, "[TRIBALIA GAME READY]", 21) &&
			     cli->IsReady()) ||
			    (!strncmp(m, "[TRIBALIA GAME NOTREADY]", 21) &&
			     !cli->IsReady())) {

			    cli->GetQueue()->ReceiveTCP(m, 40);
			    Log::GetLog()->Warning("server", "%s set ready status more than one time -- possible flooding?", cli->GetName());
			    continue;
			}

			if (!strncmp(m, "[TRIBALIA GAME NOTREADY]", 21) && cli->IsReady()) {
			    cli->GetQueue()->ReceiveTCP(m, 40);
			    printf("\033[1mClient %s isn't ready anymore\033[0m\n",
				   cli->GetName());
			    cli->UnsetReady();
			    continue;
			}
		    }

		    // Check if all clients are ready
		    bool cli_ready = true;
		    for (const auto ccli : clis) {
			if (!ccli->IsReady()) {
			    cli_ready = false;
			    break;
			}
		    }

		    // Everybody is ready, send the message
		    if (clis.size() > 0 && cli_ready && !game_starting) {
			game_starting = true;
			for (auto ccli : clis) {
			    ccli->GetQueue()->SendTCP("[TRIBALIA GAME STARTING]\n");
			    // TODO: wait before start the game
			    // TODO: docs: send a message when some user unready.
			}
		    } else {
			if (!cli_ready)
			    game_starting = false;
		    }

		    
		    
		    // Check for chats
		    memset(m, 0, 2048);
		    while (auto cm = chm->CheckMessage(cli)) {
			cl.Push(cm);
			for (auto ccli : clis) {
			    if (ccli->GetID() != cli->GetID())
				chm->SendAll(ccli, cli, cm->message);
			}
		    }

		}
		    
	    }
	    

	    clis.remove_if(
		[](Client* c){ return c->IsClosed() ||
			c->GetStatus() == CS_DISCONNECTED;
		});
	    
	    
	}
	
	delete sm;	    
    } catch (ServerManagerError& e) {
	Log::GetLog()->Fatal("main", "Error: %s", e.what());
	return 1;
    }


    printf("Exit");
    return 0;
}	

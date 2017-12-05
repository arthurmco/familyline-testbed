#include "ServerPlayerManager.hpp"

using namespace Tribalia::Server;


/* Add a client as a player
   Call this function right after the client received its ID */
void PlayerManager::RegisterClient(Client* c)
{
    _players.push_back(NetPlayer{.cli = c});
    this->NotifyPlayerAddition(c);
}

/* Process requests made by the clients, like obtaining maps and
   player lists 
*/
void PlayerManager::Process()
{
    for (const auto& player : _players) {
	
    }
}


void PlayerManager::NotifyPlayerAddition(Client* c)
{
    printf("... client %s with id %d added, notifying others\n",
	   c->GetName(), c->GetID());
}

void PlayerManager::SendPlayerList(Client* c)
{

}

void PlayerManager::SendMapList(Client* c)
{

}

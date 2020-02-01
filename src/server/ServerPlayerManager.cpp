#include "ServerPlayerManager.hpp"
#include <Log.hpp>

using namespace familyline::Server;


/* Add a client as a player
   Call this function right after the client received its ID */
void PlayerManager::RegisterClient(Client* c)
{
    _players.push_back(NetPlayer{c});
    this->NotifyPlayerAddition(c);
}

/* Process requests made by the clients, like obtaining maps and
   player lists 
*/
void PlayerManager::Process()
{
    char rstr[32];
    for (const auto player : _players) {
	if (player.cli->IsClosed() ||
	    player.cli->GetStatus() == CS_DISCONNECTED) {
	    continue;
	}
	
	if (!player.cli->GetQueue()->PeekTCP(rstr, 21))
	    continue;
	
	if (!strncmp(rstr, "[TRIBALIA PLAYERS?]", 19)) {
	    this->SendPlayerList(player.cli);
	    
	} else if (!strncmp(rstr, "[TRIBALIA MAPS?]", 16)) {
	    this->SendMapList(player.cli);
	    
	}
    }

    _players.remove_if([](auto& player){
	    return player.cli->IsClosed() ||
		player.cli->GetStatus() == CS_DISCONNECTED;
	});
}


void PlayerManager::NotifyPlayerAddition(Client* c)
{
    Log::GetLog()->Write("player-manager",
			 "client %s with id %d added, notifying others\n",
			 c->getName(), c->GetID());
}

void PlayerManager::SendPlayerList(Client* c)
{
    char rstr[32];
    c->GetQueue()->ReceiveTCP(rstr, 19);
	    
    char* pfullmsg = new char[64 * _players.size()];
    pfullmsg[0] = '\0';
    pfullmsg[1] = '\0';
	    
    for (const auto& p : _players) {	
	char playmsg[32];
	sprintf(playmsg, "| %d '%s' %d ", p.cli->GetID(),
		p.cli->getName(), 0);
	strcat(pfullmsg, playmsg);
    }

    char* sstr = new char[32 * strlen(pfullmsg)];
    sprintf(sstr, "[TRIBALIA PLAYERS %zu %s]\n", _players.size(),
	    pfullmsg);
    delete[] pfullmsg;

    c->GetQueue()->SendTCP(sstr);
    delete[] sstr;
}

void PlayerManager::SendMapList(Client* c)
{
    char rstr[32];
    c->GetQueue()->ReceiveTCP(rstr, 16);

    // We have no different maps yet, so send a basic template
    c->GetQueue()->SendTCP(
	"[TRIBALIA MAPS 1 | default 'Default map' 0 ]\n");
}

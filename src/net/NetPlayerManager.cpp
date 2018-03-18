#include "NetPlayerManager.hpp"
#include "NetServer.hpp"

using namespace Tribalia::Net;
using namespace Tribalia::Logic;

/**
 * Build a network player manager.
 *
 * @param player_name The local/human player name
 * @param player_id The local/human player id
 * @param server_mq The server message queue, to send/receive messages
 */
NetPlayerManager::NetPlayerManager(const char* player_name, int player_id,
				   Tribalia::Server::NetMessageQueue* server_mq)
    :  PlayerManager(),
       smq(server_mq)
     
{
    this->humandata = {.p = new HumanPlayer(player_name, 0),
		       .ID = player_id,
		       .flags = PlayerFlags::PlayerIsHuman};
    this->npf = new NetPlayerFilter(this->smq);
}

/* Add a player, receive its ID */
int NetPlayerManager::AddPlayer(Player* p, int flags)
{
    if (p == this->humandata.p)
	return this->humandata.ID;
    
    return PlayerManager::AddPlayer(p, flags);
}

/* Get a player by some information (ID or name) */
const Player* NetPlayerManager::GetbyID(int ID) const
{
    return PlayerManager::GetbyID(ID);
}

const Player* NetPlayerManager::GetbyName(const char* name) const
{
    return PlayerManager::GetbyName(name);
}

/* Get the human player that represents this client */
HumanPlayer* NetPlayerManager::GetHumanPlayer()
{
    return dynamic_cast<HumanPlayer*>(humandata.p);
}

/**
 * Gets information about the remote players
 */
void NetPlayerManager::GetRemotePlayers(Server* ns)
{
    this->smq->SendTCP("[TRIBALIA PLAYERS?]\n");
    usleep(50000); // Give it some time, 50ms will yield to another task

    ns->GetMessages();
    
    char msg[32];
    auto s = this->smq->PeekTCP(msg, 31);
    
    if (s > 0 && !strncmp(msg, "[TRIBALIA PLAYERS", 17)) {
	unsigned int playerqt = 0;
	char tr[10], pl[10];
	sscanf(msg, "%s %s %d", tr, pl, &playerqt);

	Log::GetLog()->Write("net-player-manager",
			     "%d players identified", playerqt);

	auto msgsize = 32 + playerqt*48;
	char* rmsg = new char[msgsize];
	
	this->smq->ReceiveTCP(rmsg, msgsize);
	
	char* playersep = strchr(rmsg, '|');
	while (playersep) {
	    rmsg = playersep+1;
	    char* npsep = strchr(rmsg, '|');

	    int playerid = 0;
	    char pname[32];
	    int playerxp = 0;
	    sscanf(rmsg, "%d %s %d", &playerid,
		   pname, &playerxp);

	    Log::GetLog()->InfoWrite("net-player-manager",
				     "\t player %s, ID %d, %d XP",
				     pname, playerid, playerxp);
	    
	    playersep = npsep;
	}
	
    }    
}

/* Process inputs of all players 
 * Returns true if any input was received
 */
bool NetPlayerManager::ProcessInputs()
{
    this->npf->Filter();
    return PlayerManager::ProcessInputs();
}

/* Gets the player message type name(args) const;ilter */
NetPlayerFilter* NetPlayerManager::GetMessageFilter()
{
    return this->npf;
}

/* Play for all users.
   Return false only if the human player returns false.
   This usually mean that the human wants to stop the game.
*/
bool NetPlayerManager::PlayAll(GameContext* gct)
{
    return PlayerManager::PlayAll(gct);
}

NetPlayerManager::~NetPlayerManager()
{
    delete npf;
}

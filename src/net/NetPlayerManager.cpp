#include "NetPlayerManager.hpp"

using namespace Tribalia::Net;
using namespace Tribalia::Logic;

NetPlayerManager::NetPlayerManager(const char* player_name, int player_id,
				   Server::ClientMessageQueue* server_mq)
    :  PlayerManager(),
       npf(new NetPlayerFilter(server_mq))
     
{
    this->humandata = {.p = new HumanPlayer(player_name, 0),
		       .ID = player_id,
		       .flags = PlayerFlags::PlayerIsHuman};
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


/* Process inputs of all players 
 * Returns true if any input was received
 */
bool NetPlayerManager::ProcessInputs()
{
    
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

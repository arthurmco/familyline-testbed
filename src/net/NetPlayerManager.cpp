#include "NetPlayerManager.hpp"

using namespace Tribalia::Net;
using namespace Tribalia::Logic;

NetPlayerManager::NetPlayerManager(const char* player_name, int player_id)
    : PlayerManager()
{
    
}

/* Add a player, receive its ID */
int NetPlayerManager::AddPlayer(Player* p, int flags)
{
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


/* Process inputs of all players 
 * Returns true if any input was received
 */
bool NetPlayerManager::ProcessInputs()
{
    return PlayerManager::ProcessInputs();
}


/* Play for all users.
   Return false only if the human player returns false.
   This usually mean that the human wants to stop the game.
*/
bool NetPlayerManager::PlayAll(GameContext* gct)
{
    return PlayerManager::PlayAll(gct);
}


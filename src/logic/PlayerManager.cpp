#include "PlayerManager.hpp"
#include "Log.hpp"

using namespace Familyline::Logic;

static int last_player_id = 0;

/* Add a player, receive its ID */
int PlayerManager::AddPlayer(Player* p, int flags)
{
    PlayerData pd = {p, ++last_player_id, flags};
    this->_playerdata.push_back(std::move(pd));

    Log::GetLog()->Write("player-manager",
			 "Added player (name: %s, id: %d, flags %04x)",
			 p->GetName(), last_player_id, flags);
    return last_player_id;
}

/* Get a player by its ID */
const Player* PlayerManager::GetbyID(int ID) const
{
    auto res = std::find_if(this->_playerdata.begin(), this->_playerdata.end(),
			    [ID](PlayerData pd) {
				return pd.ID == ID;
			    });

    return (res != this->_playerdata.end()) ? res->p : nullptr;

    
}

/* Get a player by its name */
const Player* PlayerManager::GetbyName(const char* name) const
{
    auto res = std::find_if(this->_playerdata.begin(), this->_playerdata.end(),
			    [name](PlayerData pd) {
				return !strcmp(name, pd.p->GetName());
			    });

    return (res != this->_playerdata.end()) ? res->p : nullptr;
}

/* Process inputs of all players.
   Returns true if any input was received
 */
bool PlayerManager::ProcessInputs()
{
    bool ret = false;
    for (auto& pd : this->_playerdata) {
	if (pd.p->ProcessInput())
	    ret = true;
    }

    return ret;
}

/* Play for all users.
   Return false only if the human player returns false.
   This usually mean that the human wants to stop the game.
*/
bool PlayerManager::PlayAll(GameContext* gct)
{
    bool ret = true;
    for (auto& pd : this->_playerdata) {
	bool localret = true;
	localret = pd.p->Play(gct);
	if (pd.flags & PlayerFlags::PlayerIsHuman)
	    ret = localret;
    }

    return ret;
}

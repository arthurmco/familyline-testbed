#include "PlayerManager.hpp"
#include "../Log.hpp"

using namespace Tribalia::Logic;

static int last_player_id = 0;

/* Add a player, receive its ID */
int PlayerManager::AddPlayer(Player* p)
{
    PlayerData pd = {p, ++last_player_id};
    this->_playerdata.push_back(std::move(pd));

    Log::GetLog()->Write("player-manager",
			 "Added player (name: %s, id: %d)",
			 p->GetName(), last_player_id);
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

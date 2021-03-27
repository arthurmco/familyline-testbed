#pragma once

/**
 * Manages players and player input events
 *
 * Copyright (C) 2020 Arthur M.
 */

#include <common/logic/PathFinder.hpp>
#include <common/logic/lifecycle_manager.hpp>
#include <common/logic/player.hpp>
#include <common/logic/player_actions.hpp>
#include <functional>
#include <memory>
#include <queue>
#include <vector>
#include <map>

namespace familyline::logic
{
struct PlayerInfo {
    int id;
    std::unique_ptr<Player> player;

    PlayerInfo(int id, std::unique_ptr<Player> player) : id(id), player(std::move(player)) {}
};

using PlayerListenerHandler = std::function<bool(PlayerInputAction)>;

struct PlayerHandlerInfo {
    int id;
    PlayerListenerHandler handler;
};

using PlayerCallback = std::function<void(Player*)>;

class PlayerManager
{
private:
    std::vector<PlayerInfo> players_;

    std::queue<PlayerInputAction> actions_;
    std::vector<PlayerHandlerInfo> player_input_listeners_;

    unsigned int _tick = 0;

    void processAction(const PlayerInputAction& a, ObjectManager& om);

    std::optional<Player*> getPlayerFromID(int id);

public:
    ObjectLifecycleManager* olm = nullptr;
    PathFinder* pf              = nullptr;

    /* Callback to a function that allows us to add
     * an object to the renderer
     */
    std::function<void(std::shared_ptr<GameObject>)> render_add_callback;

    /**
     * Callback to a function that allows us to
     * add an object to a colony
     */
    std::function<void(std::shared_ptr<GameObject>, unsigned /*player_id*/)> colony_add_callback;

    /**
     * Add a player here
     *
     * Return its generated ID
     */
    int add(std::unique_ptr<Player> p, bool allocate_id = true);

    /**
     * Get a player from the player manager
     *
     * Returns the player object.
     * Remember that this object is owned by the player manager.
     * In C++, you are the borrow checker.
     */
    std::optional<Player*> get(int id);

    /**
     * Gets a multimap of ID=>playername, so you can easily discover the
     * ID of a certain player name, or a name of a player who has a
     * certain ID
     */
    std::multimap<int, std::string> getPlayerNames();
    
    /**
     * Push an action
     */
    void pushAction(unsigned int id, PlayerInputType type);

    /**
     * Adds a listener to the player input action event listeners
     *
     * Returns the ID
     */
    int addListener(PlayerListenerHandler h);

    /**
     * Generate input from all players
     */
    void generateInput();

    /**
     * Exit was requested by some player
     *
     * This can only be requested by the human player.
     */
    bool exitRequested();

    /**
     * Run the input handlers and pop the event from the input action
     * queue
     */
    void run(GameContext& gctx);

    /**
     * Iterate between the players, allows the game interface to iterate on the player
     * list
     */
    void iterate(PlayerCallback c);

    /**
     * Get the current tick, as registered by the player manager
     */
    size_t tick() const { return _tick; }
};

}  // namespace familyline::logic

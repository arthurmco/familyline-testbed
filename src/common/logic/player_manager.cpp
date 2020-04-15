#include <common/logic/player_manager.hpp>
#include <common/logic/logic_service.hpp>
#include <common/logger.hpp>
#include <algorithm>
#include <chrono>
#include <fmt/format.h>
#include <cassert>

using namespace familyline::logic;

/**
 * Add a player here
 *
 * Return its generated ID
 */
int PlayerManager::add(std::unique_ptr<Player> p, bool allocate_id)
{
    auto& pi = players_.emplace_back((uintptr_t)p.get() / 1+(((uintptr_t)players_.size()*16384)),
                                     std::move(p));

    if (allocate_id)
        pi.player->code_ = pi.id;

    return pi.id;
}

std::optional<Player*> PlayerManager::getPlayerFromID(int id)
{
    auto p = std::find_if(players_.begin(), players_.end(),
                       [&](PlayerInfo& pi) {
                           return pi.id == id;
                       });

    if (p == players_.end()) {
        return std::optional<Player*>();
    }

    return std::optional<Player*>(p->player.get());
}


/**
 * Push an action
 */
void PlayerManager::pushAction(int id, PlayerInputType type)
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
	uint64_t micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

    PlayerInputAction a;
    a.playercode = id;
    a.tick = _tick;
    a.timestamp = micros;
    a.type = type;

    actions_.push(a);
}

/**
 * Adds a listener to the player input action event listeners
 *
 * Returns the ID
 */
int PlayerManager::addListener(PlayerListenerHandler h)
{
    PlayerHandlerInfo phi;
    phi.id = player_input_listeners_.size() + 1;
    phi.handler = h;

    player_input_listeners_.push_back(phi);
    return phi.id;
}

void PlayerManager::processAction(const PlayerInputAction& pia, ObjectManager& om)
{
    fmt::memory_buffer out;
    format_to(out, "action of player {:x} at tick {:d}",
              pia.playercode, pia.tick);

    struct InputVisitor {
        std::optional<Player*> pl;
        fmt::memory_buffer& out;
        ObjectManager& om;
        ObjectLifecycleManager& olm;
        std::function<void(std::shared_ptr<GameObject>)> render_add_cb;
        
        void operator()(EnqueueBuildAction a) {
            auto& log = LoggerService::getLogger();
            log->write("player-manager", LogType::Debug,
                       "%s type: EnqueueBuildAction: typename %s",
                       fmt::to_string(out).data(),
                       a.type_name.c_str());
            
            if (this->pl.has_value()) {
                (*this->pl)->pushNextBuilding(a.type_name);
            }
            
        }
        void operator()(CommitLastBuildAction a) {
            auto& log = LoggerService::getLogger();
            log->write("player-manager", LogType::Debug,
                       "%s type: CommitLastBuildAction: pos: %.2f, %.2f, last: %s",
                       fmt::to_string(out).data(),
                       a.destX, a.destZ, a.last_build ? "true" : "false");

            if (this->pl.has_value()) {
                auto player = (*this->pl);
                auto building = player->getNextBuilding();

                if (building.has_value()) {
                    auto& of = LogicService::getObjectFactory();
                    auto nobj = std::dynamic_pointer_cast<GameObject>(
                        of->getObject(building->c_str(), 0, 0, 0));

                    if (!nobj) {
                        log->write("player-manager", LogType::Error,
                                   "building type %s not found", building->c_str());
                        return;
                    }

                    glm::vec3 buildpos(a.destX, a.destY, a.destZ);
                    nobj->setPosition(buildpos);

                    auto cobjID = om.add(std::move(nobj));
                    auto ncobj =  om.get(cobjID).value();

                    assert(ncobj->getPosition().x == buildpos.x);
                    assert(ncobj->getPosition().z == buildpos.z);

                    render_add_cb(ncobj);

                    olm.doRegister(ncobj);
                    olm.notifyCreation(cobjID);

                }
            }
        }
        void operator()(ObjectSelectAction a) {
            auto& log = LoggerService::getLogger();
            log->write("player-manager", LogType::Debug,
                       "%s type: ObjectSelectAction: id %ld",
                       fmt::to_string(out).data(),
                       a.objectID);
        }
        void operator()(ObjectMoveAction a) {
            auto& log = LoggerService::getLogger();
            log->write("player-manager", LogType::Debug,
                       "%s type: ObjectMoveAction: move selected object to %.2f,%.2f",
                       fmt::to_string(out).data(),
                       a.destX, a.destZ);
        }
        void operator()(ObjectUseAction a) {
            auto& log = LoggerService::getLogger();
            log->write("player-manager", LogType::Debug,
                       "%s type: ObjectUseAction: make selected object use object %ld",
                       fmt::to_string(out).data(),
                       a.useWhat);
        }
        void operator()(ObjectRunAction a) {
            auto& log = LoggerService::getLogger();
            log->write("player-manager", LogType::Debug,
                       "%s type: ObjectRunAction: make selected object run action %s",
                       fmt::to_string(out).data(),
                       a.actionName.c_str());
        }
        void operator()(CameraMove a) {
            auto& log = LoggerService::getLogger();
            log->write("player-manager", LogType::Debug,
                       "%s type: CameraMove: dx: %.2f, dy: %.2f, dZoom: %.3f",
                       fmt::to_string(out).data(),
                       a.deltaX, a.deltaY, a.deltaZoom);

            if (this->pl.has_value()) {
                auto optcam = (*this->pl)->getCamera();
                if (optcam.has_value()) {

                    glm::vec3 mov(a.deltaX, 0, a.deltaY);

                    (*optcam)->AddPosition(mov);
                    (*optcam)->AddLookAt(mov);                    
                }
            }            
        }

    };
    std::visit(InputVisitor{this->getPlayerFromID(pia.playercode), out, om,
            *olm, this->render_add_callback},
        pia.type);
}


/**
 * Exit was requested by some player
 *
 * This can only be requested by the human player.
 */
bool PlayerManager::exitRequested()
{
    bool r = false;

    for (auto& p : players_) {
        r = r || p.player->exitRequested();
    }

    return r;
}

/**
 * Generate input from all players
 */
void PlayerManager::generateInput()
{
    for (auto& p : players_) {
        p.player->generateInput();
    }
}

/**
 * Run the input handlers and pop the event from the input action
 * queue
 */
void PlayerManager::run(GameContext& gctx)
{
    _tick = gctx.tick;
    if (!actions_.empty()) {
        PlayerInputAction& a = actions_.front();
        this->processAction(a, *gctx.om);

        for (auto h : player_input_listeners_) {
            h.handler(a);
        }

        actions_.pop();
    }
}

#include <fmt/format.h>

#include <algorithm>
#include <vector>
#include <cassert>
#include <chrono>
#include <cinttypes>
#include <common/logger.hpp>
#include <common/logic/colony.hpp>
#include <common/logic/logic_service.hpp>
#include <common/logic/object_path_manager.hpp>
#include <common/logic/player_manager.hpp>

/**
 * TODO: create an object that will create other objects.
 *
 * it will be used now to create our test objects in the map, but in the future it will
 * be used to create objects in scripts, or in triggers, or in the start of the game.
 */

using namespace familyline;
using namespace familyline::logic;

/**
 * Add a player here
 *
 * Return its generated ID
 */
uint64_t PlayerManager::add(std::unique_ptr<Player> p, bool allocate_id)
{
    auto id = allocate_id ? (uintptr_t)p.get() / 1 + (((uintptr_t)players_.size() * 16384))
                          : p->getCode();

    auto& pi         = players_.emplace_back(id, std::move(p));
    pi.player->code_ = pi.id;

    return pi.id;
}

/**
 * Get a player from the player manager
 *
 * Returns the player object.
 * Remember that this object is owned by the player manager.
 * In C++, you are the borrow checker.
 */
std::optional<Player*> PlayerManager::get(uint64_t id) { return this->getPlayerFromID(id); }

std::optional<Player*> PlayerManager::getPlayerFromID(uint64_t id)
{
    auto p =
        std::find_if(players_.begin(), players_.end(), [&](PlayerInfo& pi) { return pi.id == id; });

    if (p == players_.end()) {
        return std::optional<Player*>();
    }

    return std::optional<Player*>(p->player.get());
}

/**
 * Iterate between the players, allows the game interface to iterate on the player
 * list
 */
void PlayerManager::iterate(PlayerCallback c)
{
    std::for_each(players_.begin(), players_.end(), [&](auto& p) { c(p.player.get()); });
}

/**
 * Push an action
 *
 * We can push an action to be ran in a certain tick
 */
void PlayerManager::pushAction(uint64_t id, PlayerInputType type, std::optional<unsigned int> tick)
{
    auto duration   = std::chrono::system_clock::now().time_since_epoch();
    uint64_t micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

    auto& log    = LoggerService::getLogger();
    auto runtick = tick ? *tick : tick_ + tick_delta_;

    log->write(
        "player-manager", LogType::Info, "push action of player {:16x} on tick {} to be run on {}",
        id, tick_, runtick);

    if (tick) {
        assert(*tick >= tick_);
    }
    
    PlayerInputAction a;
    a.playercode = id;
    a.tick       = runtick;
    a.timestamp  = micros;
    a.type       = type;

    actions_.push_back(a);

    
    for (auto h : player_input_listeners_add_) {
        h.handler(a);
    }
    
    std::sort(
        actions_.begin(), actions_.end(),
        [&](const PlayerInputAction& a, const PlayerInputAction& b) {
            // return a < b

            if (a.tick != b.tick) return a.tick < b.tick;

            return a.timestamp < b.timestamp;
        });
}

auto getValidSelections(const std::vector<std::weak_ptr<GameEntity>>& selections)
{
    // TODO: PLEASE use ranges instead of this.
    std::vector<std::weak_ptr<GameEntity>> temp_selections;
    std::vector<std::shared_ptr<GameEntity>> valid_selections;

    std::copy_if(
        selections.begin(), selections.end(), std::back_inserter(temp_selections), [](auto& sel) {
            if (sel.expired()) return false;

            return true;
        });

    std::transform(
        temp_selections.begin(), temp_selections.end(), std::back_inserter(valid_selections),
        [](auto& sel) { return sel.lock(); });

    return valid_selections;
}

/**
 * Adds a listener to the player input action event listeners
 *
 * Returns the ID
 *
 * Note that, if you add a listener for when an action add event, they might not be
 * in order. The action run event is guaranteed to be in order
 */
int PlayerManager::addListener(PlayerListenerHandler h, PlayerHandlerType type)
{
    static int id = 1;    
    
    PlayerHandlerInfo phi;
    phi.id      = id++;
    phi.handler = h;

    switch (type) {
    case PlayerHandlerType::AddHandler:
        player_input_listeners_add_.push_back(phi);
        break;
    case PlayerHandlerType::RunHandler:
        player_input_listeners_run_.push_back(phi);
        break;
    }

    return phi.id;
}

/**
 * Removes the player input action event listener
 */
void PlayerManager::removeListener(int id)
{
    auto it = std::erase_if(
        player_input_listeners_add_, [id](const PlayerHandlerInfo& i) { return i.id == id; });

    auto _it = std::erase_if(
        player_input_listeners_run_, [id](const PlayerHandlerInfo& i) { return i.id == id; });
}

/// This will allow us to use std::visit with multiple variants at once, a thing
/// that should be part of C++20.
/// There are three locations that I use this. I hope this gets suggested for C++23
/// or C++26
template <class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

void writeSelectDebugInfo(std::unique_ptr<Logger>& log, const char* str, const SelectAction& a)
{
    switch (a.objects.size()) {
        case 0:
            log->write(
                "player-manager", LogType::Info, "{}: SelectAction< objects=0, deselecting all > ",
                str);
            break;
        case 1:
            log->write(
                "player-manager", LogType::Info, "{}: SelectAction< objects=[{}] > ", str,
                a.objects[0]);
            break;
        case 2:
            log->write(
                "player-manager", LogType::Info, "{}: SelectAction< objects=[{}, {}] > ", str,
                a.objects[0], a.objects[1]);
            break;
        case 3:
            log->write(
                "player-manager", LogType::Info, "{}: SelectAction< objects=[{}, {}, {}] > ",
                str, a.objects[0], a.objects[1], a.objects[2]);
            break;
        default:
            log->write(
                "player-manager", LogType::Info,
                "{}: SelectAction < objects={} > ", str,
                a.objects);
            break;
    }
}

void PlayerManager::processAction(const PlayerInputAction& pia, ObjectManager& om)
{
    fmt::memory_buffer out;
    format_to(out, "action of player {:x} at tick {:d}", pia.playercode, pia.tick);

    assert(olm != nullptr);

    auto& log = LoggerService::getLogger();

    char str[96];
    sprintf(str, "receive action of player %" PRIx64 " at tick %" PRId32, pia.playercode, pia.tick);

    auto player        = this->getPlayerFromID(pia.playercode);
    bool invalidAction = false;

    if (!player.has_value()) {
        log->write(
            "player-manager", LogType::Fatal, "invalid player ID ({})!", pia.playercode);
        return;
    }

    std::visit(
        overload{
            [&](const CommandInput& a) {
                /**
                 * The attack command is the only command treated specially.
                 */
                log->write(
                    "player-manager", LogType::Info, "{}: CommandInput< command={} > ", str,
                    a.commandName);

                if (a.commandName == "attack") {
                    auto* objectID = std::get_if<object_id_t>(&a.param);
                    if (!objectID) {
                        log->write(
                            "player-manager", LogType::Error,
                            "invalid attackee (eventparamtype={})", a.param.index());
                        invalidAction = true;
                        return;
                    }

                    auto attacker_w = (*player)->getSelections().at(0);
                    auto attackee_o = om.get(*objectID);

                    if (!attacker_w.expired() && attackee_o.has_value()) {
                        auto attacker = attacker_w.lock();
                        auto attackee = (*attackee_o);

                        if (attacker->getColonyComponent().has_value() &&
                            attacker->getColonyComponent()->owner.has_value() &&
                            attacker->getColonyComponent()->owner->get().isOfPlayer(*(*player))) {

                            if (attacker->getAttackComponent() && attackee->getAttackComponent())
                                attacker->getAttackComponent()->attack(*attackee->getAttackComponent());

                        }
                    } else {
                        log->write(
                            "player-manager", LogType::Error,
                            "invalid attackee (ID {} or its target could not be found)", *objectID);
                    }

                } else if (a.commandName == "null") {
                    log->write(
                        "player-manager", LogType::Warning,
                        "null command, maybe the recorded player did not recoginize");

                } else {
                    auto selections       = (*player)->getSelections();
                    auto valid_selections = getValidSelections(selections);

                    for (auto& s : valid_selections) {
                        if (s->hasCommand(a.commandName)) {
                            switch (a.param.index()) {
                                case 0: s->runCommand(a.commandName, {}); break;
                                case 1: {
                                    auto object = std::get<object_id_t>(a.param);
                                    s->runCommand(a.commandName, {object});
                                    break;
                                }
                                case 2: {
                                    auto pos = std::get<std::array<int, 2>>(a.param);
                                    s->runCommand(
                                        a.commandName,
                                        {(unsigned long long)pos[0], (unsigned long long)pos[1]});
                                    break;
                                }
                            }
                        }
                    }
                }
            },
            [&](const SelectAction& a) {
                writeSelectDebugInfo(log, str, a);

                (*player)->clearSelection();

                if (a.objects.size() > 0) {
                    auto obj = om.get(a.objects[0]);
                    if (!obj) {
                        invalidAction = true;
                    } else {
                        (*player)->pushToSelection(a.objects[0], *obj);
                    }
                }
            },
            [&](const ObjectMove& a) {
                log->write(
                    "player-manager", LogType::Info, "{}: ObjectMove< xPos={}, yPos={} >", str,
                    a.xPos, a.yPos);

                auto selections       = (*player)->getSelections();
                auto valid_selections = getValidSelections(selections);

                for (auto& s : valid_selections) {
                    // Only move components that are of the player.
                    if (s->getColonyComponent().has_value() &&
                        s->getColonyComponent()->owner.has_value() &&
                        s->getColonyComponent()->owner->get().isOfPlayer(*(*player))) {
                        auto& pm = LogicService::getPathManager();
                        pm->startPathing(*s.get(), glm::vec2(a.xPos, a.yPos));

                        log->write(
                            "human-player", LogType::Debug, "moved to {}, {}", a.xPos, a.yPos);
                    }
                }
            },
            [&](const CameraMove& a) {
                log->write(
                    "player-manager", LogType::Info,
                    "{}: CameraMove< dx={:.2f}, dy={:.2f}, dZoom={:.2f}> ", str, a.deltaX, a.deltaY,
                    a.deltaZoom);

                auto optcam = (*player)->getCamera();
                if (optcam.has_value()) {
                    glm::vec3 mov(a.deltaX, 0, a.deltaY);

                    (*optcam)->AddPosition(mov);
                    (*optcam)->AddLookAt(mov);
                    (*optcam)->AddZoomLevel(a.deltaZoom);
                }
            },
            [&](const CameraRotate& a) {
                log->write(
                    "player-manager", LogType::Info, "{}: CameraRotate< angle={:.3f} rad > ", str,
                    a.radians);

                auto optcam = (*player)->getCamera();
                if (optcam.has_value()) {
                    (*optcam)->AddRotation(glm::vec3(0, 1, 0), a.radians);
                }
            },
            [&](const CreateEntity& a) {                
                log->write(
                    "player-manager", LogType::Info,
                    "{}: CreateEntity< type={}, xPos={}, yPos={} > ", str, a.type, a.xPos, a.yPos);

                auto& of = LogicService::getObjectFactory();
                auto nobj =
                    std::dynamic_pointer_cast<GameEntity>(of->getObject(a.type.c_str(), 0, 0, 0));

                if (!nobj) {
                    log->write(
                        "player-manager", LogType::Error, "building type {} not found",
                        a.type.c_str());
                    invalidAction = false;
                    return;
                }

                auto height = (*player)->terr_.getHeightFromCoords(glm::vec2(a.xPos, a.yPos));
                glm::vec3 buildpos(a.xPos, height, a.yPos);
                nobj->setPosition(buildpos);

                auto cobjID = om.add(std::move(nobj));
                auto ncobj  = om.get(cobjID).value();

                assert(ncobj->getPosition().x == buildpos.x);
                assert(ncobj->getPosition().z == buildpos.z);

                this->render_add_callback(ncobj);

                olm->doRegister(ncobj);
                colony_add_callback(ncobj, (*player)->getCode());
                olm->notifyCreation(cobjID);
            },
            [&](const AddSelectAction& a) {}, [&](const CreateSelectGroup& a) {},
            [&](const SelectGroup& a) {}, [&](const AddSelectGroup& a) {},
            [&](const RemoveSelectGroup& a) {}},
        pia.type);

    if (invalidAction) {
        log->write("player-manager", LogType::Fatal, "last action had an invalid parameter!");
        log->write("player-manager", LogType::Fatal, "This WILL cause issues later!");
    }
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
 * Gets a multimap of ID=>playername, so you can easily discover the
 * ID of a certain player name, or a name of a player who has a
 * certain ID
 */
std::multimap<uint64_t, std::string> PlayerManager::getPlayerNames()
{
    std::multimap<uint64_t, std::string> r;

    std::for_each(players_.begin(), players_.end(), [&r](PlayerInfo& i) {
        r.emplace(i.id, std::string{i.player->getName()});
    });
    return r;
}

/**
 * Run the input handlers and pop the event from the input action
 * queue
 */
void PlayerManager::run(GameContext& gctx)
{
    tick_ = gctx.tick;

    while (!actions_.empty()) {
        PlayerInputAction& a = actions_.front();
        if (a.tick > tick_) break;

        this->processAction(a, *gctx.om);

        for (auto h : player_input_listeners_run_) {
            h.handler(a);
        }

        actions_.pop_front();
    }
}

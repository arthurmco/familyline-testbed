#include <fmt/format.h>

#include <algorithm>
#include <common/logger.hpp>
#include <common/logic/action_queue.hpp>
#include <functional>
#include <cassert>

using namespace familyline::logic;

void ActionQueue::addEmitter(EventEmitter* e)
{
    auto& log = LoggerService::getLogger();
    log->write("action-queue", LogType::Debug, "added event emitter {}", e->getName());
    e->queue = this;
}

void ActionQueue::addReceiver(std::string name, EventReceiver r,
                              std::initializer_list<ActionQueueEvent> events)
{
    assert(r);
    auto& log = LoggerService::getLogger();
    
    log->write("action-queue", LogType::Debug, "added event receiver {}", name);

    this->receivers.emplace_back(name, r, events);
}

void ActionQueue::removeReceiver(std::string name)
{
    auto& log = LoggerService::getLogger();

    auto newend = std::remove_if(receivers.begin(), receivers.end(), [name](ReceiverData rec) {
        return (rec.name == name);
    });
    log->write("action-queue", LogType::Debug, "removed event receiver {}", name);
    receivers.erase(newend, receivers.end());
}

void ActionQueue::removeEmitter(EventEmitter* e) { e->queue = nullptr; }

/// This will allow us to use std::visit with multiple variants at once, a thing
/// that should be part of C++20.
template <class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

void ActionQueue::pushEvent(const EntityEvent& ev)
{
    std::string begin = fmt::format(
        "timestamp={}, source={}", ev.timestamp,
        ev.emitter ? ev.emitter->getName() : "(null emitter)");

    auto& log = LoggerService::getLogger();
    std::visit(
        overload{
            [&](const EventCreated& e) {
                log->write(
                    "action-queue", LogType::Info, "event added: EventCreated ({}, objectID={})",
                    begin, e.objectID);
            },
            [&](const EventBuilding& e) {
                log->write(
                    "action-queue", LogType::Info, "event added: EventBuilding ({}, objectID={})",
                    begin, e.objectID);
            },
            [&](const EventBuilt& e) {
                log->write(
                    "action-queue", LogType::Info, "event added: EventBuilt ({}, objectID={})",
                    begin, e.objectID);
            },
            [&](const EventReady& e) {
                log->write(
                    "action-queue", LogType::Info, "event added: EventReady ({}, objectID={})",
                    begin, e.objectID);
            },
            [&](const EventAttackStart& e) {
                log->write(
                    "action-queue", LogType::Info,
                    "event added: EventAttackStart ({}, "
                    "attacker(id={}, xpos={}, ypos={}), defender(id={}, xpos={}, ypos={}))",
                    begin, e.attackerID, e.atkXPos, e.atkYPos, e.defenderID, e.defXPos,
                    e.defYPos);
            },
            [&](const EventAttackMiss& e) {
                log->write(
                    "action-queue", LogType::Info,
                    "event added: EventAttackMiss ({}, "
                    "attacker(id={}, xpos={}, ypos={}), defender(id={}, xpos={}, ypos={}))",
                    begin, e.attackerID, e.atkXPos, e.atkYPos, e.defenderID, e.defXPos,
                    e.defYPos);
            },
            [&](const EventAttackDone& e) {
                log->write(
                    "action-queue", LogType::Info,
                    "event added: EventAttackDone ({}, "
                    "attacker(id={}, xpos={}, ypos={}), defender(id={}, xpos={}, ypos={}),"
                    "damageDealt={:.2f})",
                    begin, e.attackerID, e.atkXPos, e.atkYPos, e.defenderID, e.defXPos,
                    e.defYPos, e.damageDealt);
            },
            [&](const EventAttacking& e) {
                log->write(
                    "action-queue", LogType::Info,
                    "event added: EventAttacking ({}, "
                    "attacker(id={}, xpos={}, ypos={}), defender(id={}, xpos={}, ypos={}),"
                    "damageDealt={:.2f})",
                    begin, e.attackerID, e.atkXPos, e.atkYPos, e.defenderID, e.defXPos,
                    e.defYPos, e.damageDealt);
            },
            [&](const EventWorking& e) {
                log->write(
                    "action-queue", LogType::Info,
                    "event added: EventWorking ({}, objectID={},"
                    "atkXPos={}, atkYPos={}",
                    begin, e.objectID, e.atkXPos, e.atkYPos);
            },
            [&](const EventGarrisoned& e) {
                log->write(
                    "action-queue", LogType::Info,
                    "event added: EventGarrisoned ({}, objectID={},"
                    "parentID={}, entering={}",
                    begin, e.objectID, e.parentID, e.entering ? "true" : "false");
            },
            [&](const EventDying& e) {
                log->write(
                    "action-queue", LogType::Info,
                    "event added: EventDying ({}, objectID={},"
                    "atkXPos={}, atkYPos={}",
                    begin, e.objectID, e.atkXPos, e.atkYPos);
            },
            [&](const EventDead& e) {
                log->write(
                    "action-queue", LogType::Info, "event added: EventDead ({}, objectID={})",
                    begin, e.objectID);
            },
            [&](const EventDestroyed& e) {
                log->write(
                    "action-queue", LogType::Info,
                    "event added: EventDestroyed ({}, objectID={})", begin, e.objectID);
            }},
        ev.type);

    this->events.push(ev);
}

void ActionQueue::processEvents()
{
    while (!this->events.empty()) {
        EntityEvent e = this->events.front();

        // printf("event %x !\n", e.type);

        for (auto& rec : this->receivers) {
            if (std::find(rec.events.begin(), rec.events.end(), e.type.index()) !=
                rec.events.end()) {
                // printf("\t received by %s\n", rec.receiver->getName().c_str());
                rec.receiver(e);
            }
        }

        this->events.pop();
    }
}

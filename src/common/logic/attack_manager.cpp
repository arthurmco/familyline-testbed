#include <algorithm>
#include <common/logger.hpp>
#include <common/logic/action_queue.hpp>
#include <common/logic/attack_manager.hpp>
#include <common/logic/game_event.hpp>
#include <common/logic/game_object.hpp>
#include <common/logic/logic_service.hpp>

using namespace familyline::logic;

AttackManager::AttackManager()
{
    using namespace std::placeholders;

    LogicService::getActionQueue()->addReceiver(
        "attack-manager", std::bind(&AttackManager::receiveAttackEvents, this, _1),
        {ActionQueueEvent::AttackStart});

    LogicService::getActionQueue()->addEmitter(&emitter_);
}

AttackManager::~AttackManager()
{
    LogicService::getActionQueue()->removeReceiver("attack-manager");
    LogicService::getActionQueue()->removeEmitter(&emitter_);
}

/**
 * Damage the game object.
 *
 * Return its remaining health points
 */
double AttackManager::damageObject(GameObject& o, double damage)
{
    return o.addHealth(-damage);
}

void AttackManager::sendAttackDoneEvent(
    const AttackInfo& atk, AttackAttributes atkAttribute, double damageDealt)
{
    auto atkpos = atk.atkPosition;
    auto defpos = atk.defPosition;

    EntityEvent ev{
        0,
        EventAttackDone{
            .attackerID   = atk.attackerID,
            .defenderID   = atk.defenderID,
            .attackID     = atk.attackID,
            .atkXPos      = (unsigned)atkpos.x,
            .atkYPos      = (unsigned)atkpos.y,
            .defXPos      = (unsigned)defpos.x,
            .defYPos      = (unsigned)defpos.y,
            .atkAttribute = atkAttribute,
            .damageDealt  = damageDealt},
        nullptr};
    emitter_.pushEvent(ev);
}

void AttackManager::sendAttackMissEvent(const AttackInfo& atk)
{
    auto atkpos = atk.atkPosition;
    auto defpos = atk.defPosition;

    EntityEvent ev{
        0,
        EventAttackMiss{
            .attackerID = atk.attackerID,
            .defenderID = atk.defenderID,
            .attackID   = atk.attackID,
            .atkXPos    = (unsigned)atkpos.x,
            .atkYPos    = (unsigned)atkpos.y,
            .defXPos    = (unsigned)defpos.x,
            .defYPos    = (unsigned)defpos.y},
        nullptr};
    emitter_.pushEvent(ev);
}

void AttackManager::sendDyingEvent(const AttackInfo& atk)
{
    auto defpos = atk.defPosition;

    EntityEvent ev{
        0,
        EventDying{
            .objectID = atk.defenderID,
            .atkXPos  = (unsigned)defpos.x,
            .atkYPos  = (unsigned)defpos.y},
        nullptr};
    emitter_.pushEvent(ev);
}

void AttackManager::updateAttackInfo(
    const GameObject& attacker, const GameObject& defender, AttackInfo& atk)
{
    auto atkpos     = attacker.getPosition();
    auto defpos     = defender.getPosition();
    atk.atkPosition = glm::vec2(atkpos.x, atkpos.z);
    atk.defPosition = glm::vec2(defpos.x, defpos.z);
}

/**
 * Based on the specified attack precision, check if the
 * next attack will happen
 *
 * TODO: use a global pseudo-RNG. Also make every game have a seed.
 */
bool AttackManager::isNextAttackPrecise(unsigned precision) const
{
    // clang-format off
    constexpr std::array values = {
        0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
        1, 11, 21, 31, 41, 51, 61, 71, 81, 91, 0,
        2, 12, 22, 32, 42, 52, 62, 72, 82, 92, 1,
    };

    // clang-format on

    static int i = 0;

    bool answer = (values[i]) <= precision;
    i++;

    if (i >= values.size()) i = 0;

    return answer;
}

/**
 * Start a new countdown to the next attack, by incrementing
 * the ticks_until_attack by the attack interval.
 */
void AttackManager::countUntilNextAttack(AttackInfo& atk)
{
    auto interval = this->getAttackInterval(atk.atkAttributes);
    atk.ticks_until_attack += interval;
}

/**
 * Do the damage, update the projectile positions and other multiple things
 *
 * Runs once per tick
 */
void AttackManager::update(ObjectManager& om, ObjectLifecycleManager& olm)
{
    std::vector<uint64_t> attacks_to_remove;
    auto& log = LoggerService::getLogger();

    //    printf("%zu attacks\n", attacks_.size());
    for (auto& atk : attacks_) {
        auto atkobj = om.get(atk.attackerID);
        auto defobj = om.get(atk.defenderID);

        if (!defobj || !atkobj) {
            log->write(
                "attack-manager", LogType::Info,
                "attack {} ended because one of the participants do not exist anymore (atk={}, "
                "def={})",
                atk.attackID, atk.attackerID, atk.defenderID);
            attacks_to_remove.push_back(atk.attackID);
            continue;
        }

        if (!(*atkobj)->getAttackComponent()->isInRange(*defobj->get()->getAttackComponent())) {
            log->write(
                "attack-manager", LogType::Info,
                "attack {} ended because one of the participants is not in range of the other "
                "(atk={}, def={})",
                atk.attackID, atk.attackerID, atk.defenderID);
            attacks_to_remove.push_back(atk.attackID);
            continue;
        }
        
        double damage = AttackComponent::calculateDamage(atk.atkAttributes, atk.defAttributes);

        this->updateAttackInfo(*atkobj->get(), *defobj->get(), atk);

        atk.ticks_until_attack--;
        if (atk.ticks_until_attack <= 0.001) {
            int attacks = 0;
            if (isNextAttackPrecise(atk.atkAttributes.precision)) {
                // Fix some precision errors that might occur.
                atk.ticks_until_attack = 0;
                do {
                    auto remaining = this->damageObject(*defobj->get(), damage);
                    this->sendAttackDoneEvent(atk, atk.atkAttributes, damage);

                    if (remaining <= 0) {
                        this->sendDyingEvent(atk);
                        log->write(
                            "attack-manager", LogType::Info,
                            "attack {} ended because the attacker killed the defender "
                            "(atk={}, def={})",
                            atk.attackID, atk.attackerID, atk.defenderID);

                        /// The lifecycle manager will delete the entity for us,
                        /// but we will have to sent the dying event.
                        /// TODO: change this, the lifecycle manager will listen to
                        ///       this event.
                        olm.notifyDeath(atk.defenderID);
                        attacks_to_remove.push_back(atk.attackID);
                    }

                    attacks++;
                    this->countUntilNextAttack(atk);
                } while (atk.ticks_until_attack < 1.0);

                atk.successful_attacks += attacks;

            } else {
                this->sendAttackMissEvent(atk);
            }
            atk.total_attacks += attacks;
        }
    }

    auto itend = std::remove_if(attacks_.begin(), attacks_.end(), [&](AttackInfo& a) {
        return std::find(attacks_to_remove.begin(), attacks_to_remove.end(), a.attackID) !=
               attacks_to_remove.end();
    });
    attacks_.erase(itend, attacks_.end());
}

bool AttackManager::receiveAttackEvents(const EntityEvent& e)
{
    auto& log = LoggerService::getLogger();

    if (auto* ev = std::get_if<EventAttackStart>(&e.type); ev) {
        log->write(
            "attack-manager", LogType::Info, "attack event ID {:08x} received (from={}, to={})",
            ev->attackID, ev->attackerID, ev->defenderID);
        AttackInfo a{
            .attackerID         = ev->attackerID,
            .defenderID         = ev->defenderID,
            .attackID           = ev->attackID,
            .rule               = ev->rule,
            .atkAttributes      = ev->atkAttributes,
            .defAttributes      = ev->defAttributes,
            .atkPosition        = glm::vec2(ev->atkXPos, ev->atkYPos),
            .defPosition        = glm::vec2(ev->defXPos, ev->defYPos),
            .successful_attacks = 0,
            .total_attacks      = 0,
            .ticks_until_attack = 0,
        };
        attacks_.push_back(a);
    }

    return true;
}

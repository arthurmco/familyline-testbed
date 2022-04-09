#include <algorithm>
#include <array>
#include <cassert>
#include <common/logger.hpp>
#include <common/logic/action_queue.hpp>
#include <common/logic/game_event.hpp>
#include <common/logic/logic_service.hpp>
#include <common/logic/object_path_manager.hpp>
#include <common/logic/types.hpp>
#include <iterator>

using namespace familyline::logic;

ObjectPathManager::ObjectPathManager(const Terrain& t)
    : t_(t),
      base_bitmap_(std::vector<bool>(std::get<0>(t.getSize()) * std::get<1>(t.getSize()), false)),
      obj_events_(std::bind_front(&ObjectPathManager::handleEntityEvent, this))
{
    LogicService::getActionQueue()->addReceiver(
        "path-event-receiver", obj_events_,
        {
            ActionQueueEvent::Created,
            ActionQueueEvent::Dead,
            ActionQueueEvent::Destroyed,
        });
}

ObjectPathManager::~ObjectPathManager()
{
    LogicService::getActionQueue()->removeReceiver("path-event-receiver");
}

void ObjectPathManager::blockBitmapArea(int x, int y, int w, int h)
{
    auto [tw, th] = t_.getSize();

    auto bx = glm::max(x, 0);
    auto by = glm::max(y, 0);
    auto bw = glm::min(w, int(tw) - x);
    auto bh = glm::min(h, int(th) - y);

    for (y = by; y < by + bh; y++) {
        for (x = bx; x < bx + bw; x++) {
            base_bitmap_[y * tw + x] = true;
        }
    }
}

glm::vec2 to2D(glm::vec3 pos) { return glm::vec2(pos.x, pos.z); }

glm::vec3 to3D(glm::vec2 pos, const Terrain& t)
{
    return glm::vec3(pos.x, t.getHeightFromCoords(pos), pos.y);
}

glm::vec2 fixPosition(glm::vec2 pos, glm::vec2 size)
{
    return glm::vec2(
        glm::round(std::max(0.0, pos.x - (size.x / 2.0))),
        glm::round(std::max(0.0, pos.y - (size.y / 2.0))));
}

/**
 * Will this object collide with the currently pathrefs?
 *
 * (The other entities are assured not to collide because of the
 *  bitmap)
 */
ObjectPathManager::PathRef* ObjectPathManager::willCollide(
    object_id_t ourid, const std::vector<glm::vec2>& positions, glm::vec2 oursize)
{
    for (auto& ref : refs_) {
        if (ref.o->getID() == ourid) continue;

        auto otherpos = to2D(ref.o->getPosition());

        auto otherwidth  = ref.o->getSize().x;
        auto otherheight = ref.o->getSize().y;

        auto ourwidth  = oursize.x;
        auto ourheight = oursize.y;

        for (const auto& pos : positions) {
            auto fpos = fixPosition(pos, oursize);
            if (fpos.x < int(otherpos.x + otherwidth) && int(fpos.x + ourwidth) > otherpos.x &&
                fpos.y < int(otherpos.y + otherheight) && int(fpos.y + ourheight) > otherpos.y) {
                fmt::print(
                    "pos {} otherpos {} size {} othersize ({}, {})\n", fpos, otherpos, oursize,
                    otherwidth, otherheight);
                return &ref;
            }
        }
    }

    return nullptr;
}

std::vector<bool> ObjectPathManager::generateBitmap(std::vector<object_id_t> exclude_ids) const
{
    auto [tw, th]       = t_.getSize();
    std::vector<bool> b = base_bitmap_;

    for (auto& ref : refs_) {
        if (std::find(exclude_ids.begin(), exclude_ids.end(), ref.o->getID()) != exclude_ids.end())
            continue;

        auto size = ref.o->getSize();
        auto pos  = fixPosition(to2D(ref.o->getPosition()), size);

        for (int y = 0; y < th; y++) {
            if (y >= int(pos.y) && y < int(pos.y + size.y)) {
                for (int x = pos.x; x < int(pos.x + size.x); x++) {
                    b[y * tw + x] = true;
                }
            }
        }
    }

    for (auto& [sid, st] : statics_) {
        if (std::find(exclude_ids.begin(), exclude_ids.end(), sid) != exclude_ids.end()) continue;

        auto pos = fixPosition(st.position, st.size);

        for (int y = 0; y < th; y++) {
            if (y >= int(pos.y) && y < int(pos.y + st.size.y)) {
                for (int x = pos.x; x < int(pos.x + st.size.x); x++) {
                    b[y * tw + x] = true;
                }
            }
        }
    }

    return b;
}

void ObjectPathManager::update(ObjectManager& om)
{
    for (auto& ref : refs_) {
        updateRefState(ref);
    }

    for (auto& [sid, st] : statics_) {
        if (!st.valid) {
            auto obj = om.get(sid);
            if (obj) {
                st.size     = (*obj)->getSize();
                st.position = to2D((*obj)->getPosition());
                st.valid    = true;
            }
        }
    }

    refs_.erase(
        std::remove_if(
            refs_.begin(), refs_.end(),
            [&](const auto& r) {
                return std::find(to_delete_.begin(), to_delete_.end(), r.o->getID()) !=
                       to_delete_.end();
            }),
        refs_.end());
    to_delete_.clear();
}

void ObjectPathManager::doPathing(std::shared_ptr<GameObject> o, glm::vec2 destination)
{
    statics_.erase(o->getID());
    auto [tw, th] = t_.getSize();
    refs_.push_back(PathRef{
        .o                 = o,
        .previous_position = to2D(o->getPosition()),
        .positions         = {},
        .pf                = std::make_unique<Pathfinder>(generateBitmap({o->getID()}), tw, th),
        .destination       = destination});
}

bool ObjectPathManager::handleEntityEvent(const EntityEvent& e)
{
    if (auto* ec = std::get_if<EventCreated>(&e.type); ec) {
        // TODO: edit the creation event to pass initial position and size.
        statics_[ec->objectID] = StaticRef{};
    } else if (auto* ed = std::get_if<EventDestroyed>(&e.type); ed) {
        statics_.erase(ed->objectID);
    }

    return true;
}

void ObjectPathManager::updateRefState(PathRef& ref)
{
    auto& log = LoggerService::getLogger();

    if (ref.wait > 0) {
        ref.wait--;
        return;
    }

    auto [tw, th] = t_.getSize();
    switch (ref.state) {
        case PathState::Created: {
            log->write(
                "object-path-manager", LogType::Debug, "created (path={} elements)",
                ref.positions.size());
            ref.previous_position = to2D(ref.o->getPosition());

            ref.state = PathState::Pathing;
            break;
        }
        case PathState::Pathing: {
            auto path = ref.pf->calculate(to2D(ref.o->getPosition()), ref.destination);
            if (!path) {
                ref.state = PathState::ImpossiblePath;
                return;
            }

            std::copy(path->begin(), path->end(), std::back_inserter(ref.positions));

            ref.previous_position = ref.positions.front();
            ref.positions.pop_front();

            log->write(
                "object-path-manager", LogType::Debug, "pathing (path={} elements, curr={:.2f})",
                ref.positions.size(), ref.previous_position);

            ref.state = PathState::Traversing;
            break;
        }
        case PathState::Traversing: {
            log->write(
                "object-path-manager", LogType::Debug,
                "traversing (id={}, path={} elements, curr={})", ref.o->getID(),
                ref.positions.size(), ref.previous_position);

            if (auto pos2d = to2D(ref.o->getPosition()); pos2d == ref.destination) {
                ref.previous_position = pos2d;
                ref.state             = PathState::Completed;
                return;
            }

            if (ref.positions.size() == 0) {
                ref.state = PathState::Repathing;
                return;
            }

            ref.previous_position                = to2D(ref.o->getPosition());
            std::vector<glm::vec2> nextPositions = {};
            for (auto i = 0; i <= 2; i++) {
                if (ref.positions.size() > i) nextPositions.push_back(*(ref.positions.begin() + i));
            }

            if (PathRef* otherref = willCollide(ref.o->getID(), nextPositions, ref.o->getSize());
                otherref) {
                log->write(
                    "object-path-manager", LogType::Info, "{} will collide with {}, dodging...",
                    ref.o->getID(), otherref->o->getID());
                ref.state = PathState::Dodging;
                log->write(
                    "object-path-manager", LogType::Info, "{} will wait {} ticks", ref.o->getID(),
                    ref.wait);
                return;
            }

            ref.o->setPosition(to3D(nextPositions[0], t_));
            ref.positions.pop_front();

            break;
        }
        case PathState::Dodging: {
            log->write(
                "object-path-manager", LogType::Debug, "dodging (id={}, path={} elements, curr={})",
                ref.o->getID(), ref.positions.size(), to2D(ref.o->getPosition()));
            auto opos  = to2D(ref.o->getPosition());
            auto osize = ref.o->getSize();

            auto b = generateBitmap({ref.o->getID()});
            ref.pf = std::make_unique<Pathfinder>(b, tw, th);

            auto path = ref.pf->calculate(opos, ref.destination);
            if (!path || path->size() < 1) {
                ref.state = PathState::ImpossiblePath;
                return;
            }

            ref.positions.clear();
            std::copy(path->begin() + 1, path->end(), std::back_inserter(ref.positions));

            ref.state = PathState::Traversing;

            break;
        }
        case PathState::Repathing: {
            log->write("object-path-manager", LogType::Warning, "repathing is UNIMPLEMENTED!");
            assert(false);
            break;
        }
        case PathState::ImpossiblePath: {
            log->write(
                "object-path-manager", LogType::Debug,
                "impossible path (id={}, path={} elements, curr={})", ref.o->getID(),
                ref.positions.size(), ref.previous_position);
            to_delete_.push_back(ref.o->getID());
            statics_[ref.o->getID()] = StaticRef{
                .valid = true, .position = to2D(ref.o->getPosition()), .size = ref.o->getSize()};
            break;
        }
        case PathState::Died: {
            log->write(
                "object-path-manager", LogType::Debug, "died (path={} elements, curr={})",
                ref.positions.size(), ref.previous_position);
            to_delete_.push_back(ref.o->getID());
            break;
        }
        case PathState::Completed: {
            log->write(
                "object-path-manager", LogType::Debug,
                "completed (id={}, path={} elements, curr={})\n", ref.o->getID(),
                ref.positions.size(), ref.previous_position);
            to_delete_.push_back(ref.o->getID());
            statics_[ref.o->getID()] = StaticRef{
                .valid = true, .position = to2D(ref.o->getPosition()), .size = ref.o->getSize()};
            break;
        }
    }
}

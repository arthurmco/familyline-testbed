#include <array>
#include <common/logger.hpp>
#include <common/logic/pathfinder.hpp>
#include <ctime>
#include <glm/geometric.hpp>
#include <thread>

using namespace familyline::logic;

#include <algorithm>
#include <iterator>

std::unique_ptr<Pathfinder::Node> make_node(
    glm::vec2 pos, Pathfinder::Node* parent, double g, double h)
{
    Pathfinder::Node* n = new Pathfinder::Node;
    n->pos              = pos;
    n->parent           = parent;
    n->g                = g;
    n->h                = h;

    return std::unique_ptr<Pathfinder::Node>(n);
}

bool Pathfinder::isPointBlocked(glm::vec2 point) const
{
    if (point.y < 0 || point.x < 0 || point.y >= height_ || point.x >= width_) return true;

    unsigned index = int(point.y) * width_ + int(point.x);
    return bitmap_[index];
}

Pathfinder::Node* Pathfinder::tracePath(glm::vec2 from, glm::vec2 to)
{
    auto root = make_node(from, nullptr, 0, glm::abs(glm::distance(from, to)));
    current_  = root.get();
    //    printf("\x1b[033;1mcurrent: (%.2f, %.2f)\x1b[033;0m\n", current_->pos.x, current_->pos.y);
    std::swap(open_[from], root);

    while (current_->pos != to) {
        addNeighbors(*current_, to);

        auto curr     = std::min_element(open_.begin(), open_.end(), [&](auto& a, auto& b) {
            return a.second->f() < b.second->f();
            });
        auto ncurrent = std::unique_ptr<Node>();
        std::swap(curr->second, ncurrent);
        open_.erase(ncurrent->pos);
        current_ = ncurrent.get();
        closed_.push_back(std::move(ncurrent));

        if (open_.size() == 0) {
            return nullptr;
        }
    }

    return current_;
}

/**
 * Calculate a path from 'from' to 'to'
 *
 * Return every grid element we passed
 */
std::optional<std::vector<glm::vec2>> Pathfinder::calculate(glm::vec2 from, glm::vec2 to)
{
    auto& log = LoggerService::getLogger();
    log->write("pathfinder", LogType::Debug, "calculating a path from {} to {}", from, to);

#if 0
    for (auto y = 0; y < 40; y++) {
        printf("%03d |", y);
        for (auto x = 0; x < 60; x++) {
            if (x == from.x && y == from.y)
                printf("F");
            else if (x == to.x && y == to.y)
                printf("T");
            else if (bitmap_[y * width_ + x])
                printf("X");
            else
                printf(" ");
        }
        puts("|");
    }
#endif

    if (bitmap_[int(from.y) * width_ + int(from.x)]) {
        log->write(
            "pathfinder", LogType::Warning, "you are starting from an obstacle (from={}, to={})",
            from, to);
        return std::nullopt;
    }

    Node* last = tracePath(from, to);
    if (!last) {
        return std::nullopt;
    }

    std::vector<glm::vec2> path;
    path.reserve(closed_.size());

    for (; last != nullptr; last = last->parent) {
        path.push_back(last->pos);
    }

    std::reverse(path.begin(), path.end());
    log->write("pathfinder", LogType::Debug, "the generated path is {}", path);

#if 0
    for (auto y = 0; y < 40; y++) {
        printf("%03d |", y);
        for (auto x = 0; x < 60; x++) {
            if (x == from.x && y == from.y)
                printf("F");
            else if (x == to.x && y == to.y)
                printf("T");
            else if (bitmap_[y * width_ + x])
                printf("X");
            else if (std::find_if(path.begin(), path.end(), [&](glm::vec2& v) {
                return int(v.x) == x && int(v.y) == y;
            }) != path.end())
                printf(".");
            else
                printf(" ");
        }
        puts("|");
    }
#endif

    return std::make_optional(path);
}

/**
 * Add the 8 neighbors of a certain node to the open list
 *
 * If a neighbor is already in the open list, check its G score, and, if the
 * new G score is less than the calculated one, we replace its parent with
 * our parent, and recalculate.
 */
void Pathfinder::addNeighbors(Node& n, glm::vec2 to)
{
    std::vector<glm::vec2> posoffsets = {
        glm::vec2(-1, 1),
        glm::vec2(0, 1),
        glm::vec2(1, 1),
        glm::vec2(-1, 0),
        /*glm::vec2(0, 0),*/ glm::vec2(1, 0),
        glm::vec2(-1, -1),
        glm::vec2(0, -1),
        glm::vec2(1, -1)};

    glm::vec2 pos = n.pos;
    for (auto off : posoffsets) {
        if (!isPointBlocked(pos + off)) {
            auto gv     = glm::abs(glm::distance(glm::vec2(0, 0), off));
            auto gscore = n.g + gv;

            if (auto existing = open_.find(pos + off); existing != open_.end()) {
                if (existing->second->g > gscore) {
                    existing->second->parent = &n;
                    existing->second->g      = gscore;
                }
            } else {
                if (auto closedex = std::find_if(
                        closed_.begin(), closed_.end(),
                        [&](const auto& node) { return (node->pos == (pos + off)); });
                    closedex == closed_.end()) {
                    auto hscore = glm::distance(pos + off, to);
                    //                    printf("add to open [%zu]: (%.2f, %.2f), g=%.2f,
                    //                    h=%.2f\n",
                    //                           open_.size(), (pos+off).x, (pos+off).y, gscore,
                    //                           hscore);
                    open_[pos + off] = make_node(pos + off, &n, gscore, hscore);
                }
            }

        } else {
            //            printf("\x1b[033;34mBLOCKED: (%.2f, %.2f)\x1b[033;0m\n", (pos+off).x,
            //            (pos+off).y);
        }
    }
}

/**
 * Pathfinder implementation
 *
 * Copyright (C) 2021 Arthur Mendes
 */
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

// custom specialization of std::hash, so we can make hashes using
// vectors as keys
template <>
struct std::hash<glm::vec2> {
    std::size_t operator()(glm::vec2 const& v) const noexcept
    {
        static_assert(sizeof(size_t) >= 4, "Your size_t type is too low");

        size_t x = unsigned(glm::round(v.x * 100)) & (1 << (sizeof(size_t) / 4));
        size_t y = unsigned(glm::round(v.y * 100));

        return x | (y << (sizeof(size_t) * 4));
    }
};

namespace familyline::logic
{
/**
 * Basic A* pathfinder implementation
 *
 * The A* algorithm works like this:
 *    - we operate on nodes. They are like the minimum unit of path plus some info
 *    - each node has two scores:
 *       - g: the cost of the path from the starting node to the current one
 *       - h: the estimated cost of the path from the current node to the goal
 *    - also, we have f, that is g+h
 *
 *    - for each node, we get all the possible neighbor nodes (the ones that are not blocked)
 *      - each neighbor node will have the current node as a parent
 *      - we will calculate f for each node
 *      - the node with the lower score wins
 *
 *    - Every node that won is placed on the closed list. Every node that we are calculating
 *      is placed on the open list
 *    - If one of our neighbors are already in the open list, we check if our g() is less than the
 *      one that is already there
 *      - if it is, then that one has a better path. Set the parent of that one to the current node
 *      - if not, set the parent of this one to the current node.
 *
 *    If we find the target node, then we stop and backtrace our path
 *    If the open list gets empty, then we do not have a path. A fallback might be retrying, or
 * backtracing from the closest one (the one with the lower f()? )
 *
 *    (Check https://medium.com/@nicholas.w.swift/easy-a-star-pathfinding-7e6689c7f7b2 if my
 *     explanation is too hard)
 *
 *
 *   Here, we also limit the number of iterations for each pathing operation, to not calculate a
 * huge path once per frame. Do not worry, if you do not reach the destination, the point will
 * actually be the point that is closer to there.
 * This pathfinder will use a bitmap to know if the path can be traversed
 * or not.
 *
 * This pathfinder bitmap is immutable.
 * If your bitmap changed, create a new pathfinder instance.
 */
class Pathfinder
{
public:
    Pathfinder(std::vector<bool> bitmap, int width, int height)
        : bitmap_(bitmap), width_(width), height_(height)
    {
    }

    /**
     * Calculate a path from 'from' to 'to'
     *
     * Return every grid element we passed
     */
    std::optional<std::vector<glm::vec2>> calculate(glm::vec2 from, glm::vec2 to);

    struct Node {
        glm::vec2 pos;
        Node* parent = nullptr;

        // cost from begin to the current point
        double g;

        // cost from current point to the end
        double h;

        double f() const { return g + h; }
    };

private:
    std::vector<bool> bitmap_;
    int width_, height_;

    /**
     * A hashmap to store the open points
     *
     * Since this list will get very big, we need to optimize search for it,
     * and the unordered map searches faster than a vector.
     */
    std::unordered_map<glm::vec2, std::unique_ptr<Node>> open_;

    bool isPointBlocked(glm::vec2 point) const;

    /**
     * Add the 8 neighbors of a certain node to the open list
     */
    void addNeighbors(Node& n, glm::vec2 to);

    /**
     * Trace a path from 'from' to 'to'
     *
     * Return the node pointer for the position 'to'.
     * Querying the parents, you will be able to get to 'from'
     */
    Node* tracePath(glm::vec2 from, glm::vec2 to);

    std::vector<std::unique_ptr<Node>> closed_;
    Node* current_ = nullptr;
};
}  // namespace familyline::logic

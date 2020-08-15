/**
 * Colony alliance, defines diplomacy
 *
 * Copyright (C) 2020 Arthur Mendes
 */

#pragma once

#include <string>
#include <vector>

namespace familyline::logic
{
enum DiplomacyStatus { Enemy, Ally, Neutral };

/**
 * Stores an alliance
 *
 * We use a reference wrapper so we can store references in vectors
 * References cannot be reassigned, and vectors need the references to be
 * reassignable
 *
 * Since the alliances will be managed by the colony manager, we do not
 * need to use shared pointers
 */
struct Alliance {
    std::string name;
    std::vector<std::reference_wrapper<struct Alliance>> allies;
    std::vector<std::reference_wrapper<struct Alliance>> enemies;

    unsigned getID() const
    {
        unsigned val = 0;
        unsigned v2  = 0;

        for (auto c : name) {
            val |= ((unsigned)c);
            v2 = val << 8;

            val ^= 0x12345678;
            val += v2;
        }

        return val;
    }

    bool operator==(const Alliance& rhs) { return this->getID() == rhs.getID(); }
};

}  // namespace familyline::logic

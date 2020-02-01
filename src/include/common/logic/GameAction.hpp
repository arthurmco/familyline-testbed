/*
  Represents an action that happens into the game.
  Anything that players can do with objects, basically.

  Copyright (C) 2017 Arthur M
*/

#ifndef GAMEACTION_HPP
#define GAMEACTION_HPP

#include <cstdint>

namespace familyline::logic {

// Action types
enum GameActionType {
    GAT_CREATION = 1,  // Object has been created
    GAT_MOVE = 2,      // Player started moving some object
    GAT_ATTACK = 4,    // Object has been attacked
    GAT_DESTROY = 8,
};
    
struct GameAction {
    uint64_t timestamp;
    GameActionType type;
    union {
	struct {
	    int object_id;
	    float x, z;
	    int owner_id; // ID of the owning city
	} creation;
	struct {
	    int object_id;
	    float newx, newz;
	    float oldx, oldz;
	} move;
	struct {
	    int attacker_id;
	    float attackerx, attackerz;
	    int attackee_id;
	    float attackeex, attackeez;
	} attack;
	struct {
	    int object_id;
	} destroy;
    };
};

}


#endif /* GAMEACTION_HPP */

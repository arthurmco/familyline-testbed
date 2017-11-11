/*
  Represents an action that happens into the game.
  Anything that players can do with objects, basically.

  Copyright (C) 2017 Arthur M
*/

#ifndef GAMEACTION_HPP
#define GAMEACTION_HPP

#include <cstdint>

namespace Tribalia::Logic {

// Action types
enum GameActionType {
    GAT_CREATION,  // Object has been created
    GAT_MOVE,      // Player started moving some object
    GAT_ATTACK,    // Object has been attacked
    GAT_DESTROY,
};
    
struct GameAction {
    uint64_t timestamp;
    GameActionType type;
    union {
	struct {
	    int object_id;
	    float x, z;
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

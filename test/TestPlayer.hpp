#ifndef TESTPLAYER_H
#define TESTPLAYER_H

#include "logic/Player.hpp"
#include "logic/PlayerManager.hpp"
#include "logic/GameActionManager.hpp"

/* This is a test player for this test suite
   In each play loop, it increases the XP by 1

   This spares a variable creation, we can use one already.
*/
class TestPlayer : public Familyline::Logic::Player {
public:
    TestPlayer(const char* name,
	       Familyline::Logic::GameActionManager* gam =
	       new Familyline::Logic::GameActionManager{})
	: Player(name, 0, gam)
	{
	    
	}

    virtual bool Play(Familyline::Logic::GameContext*) {this->_xp++; return true; }
    virtual bool ProcessInput() { return true;}
    virtual bool HasUpdatedObject() { return true; }

};



#endif /* TESTPLAYER_H */

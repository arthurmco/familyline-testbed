/*
  Action manager

  Manages actions.
  Mostly treat with action interaction

  Copyright 2017 Arthur M
*/

#include "Action.hpp"
#include <unordered_map>
#include <algorithm>

#ifndef ACTIONMANAGER_HPP
#define ACTIONMANAGER_HPP

namespace Tribalia {
namespace Logic {

    class ActionManager {
    private:
	void RemoveVisibleAction(const char* name);
	
    protected:
	std::unordered_map<std::string, Action> _actions;
	std::vector<Action*> _visible_actions;
	
    public:	
	void AddAction(Action);
	void RemoveAction(const char* name);
	
	virtual void RunAction(const char* name) = 0;

	void SetVisibleActions(std::vector<char*> actions);
	void GetVisibleActions(std::vector<Action*>& actions) const;

	virtual ~ActionManager() {}
    };

}
}

#endif //ACTIONMANAGER_HPP

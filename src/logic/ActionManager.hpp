/*
  Action manager

  Manages actions.
  Mostly treat with interface action interaction

  Copyright (C) 2017,2018 Arthur M
*/

#include "Action.hpp"
#include <unordered_map>
#include <algorithm>

#ifndef ACTIONMANAGER_HPP
#define ACTIONMANAGER_HPP

namespace Tribalia::Logic {

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

	void SetVisibleActions(std::vector<const char*> actions);
	void GetVisibleActions(std::vector<Action*>& actions) const;

	virtual ~ActionManager() {}
    };


}

#endif //ACTIONMANAGER_HPP

#include "ActionManager.hpp"
#include "Log.hpp"
#include <cstdio>

using namespace Tribalia::Logic;

void ActionManager::AddAction(Action a)
{
    if (_actions.find(a.name.c_str()) == _actions.end()) {
	_actions[a.name.c_str()] = a;
    } else {
	_actions[a.name.c_str()].refcount++;
    }


    Log::GetLog()->Write("action-manager", "Added action %s (refcount %d)",
			 a.name.c_str(), _actions[a.name].refcount);
}

void ActionManager::RemoveVisibleAction(const char* name) {
    std::remove_if(_visible_actions.begin(), _visible_actions.end(),
		   [name](Action* act) -> bool {
		       return (!strcmp(act->name.c_str(), name)) && (act->refcount == 0);
	});
}

void ActionManager::RemoveAction(const char* name)
{
    _actions[name].refcount--;
    this->RemoveVisibleAction(name);

    for (auto act = _actions.begin(); act != _actions.end(); ) {
	if ((!strcmp(act->second.name.c_str(), name)) &&
	    (act->second.refcount == 0)) {
	    _actions.erase(act++);
	} else {
	    act++;
	}
    }
    
}
        
void ActionManager::SetVisibleActions(std::vector<char*> actions)
{
    _visible_actions.empty();
    _visible_actions.resize(actions.size());
    
    std::transform(actions.begin(), actions.end(), _visible_actions.begin(),
		   [&](char* stract) -> Action* {
		       try {
			   return &_actions.at(stract);			   
		       } catch (std::out_of_range& e) {
			   Log::GetLog()->Fatal("action-manager",
						"Action %s doesn't exist",
						stract);
			   return nullptr;
		       }
		   });
}

void ActionManager::GetVisibleActions(std::vector<Action*>& actions) const
{
    actions = _visible_actions;
}

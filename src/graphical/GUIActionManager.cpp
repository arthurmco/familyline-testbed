#include "GUIActionManager.hpp"

using namespace Familyline::Logic;
using namespace Familyline::Graphics;

GUIActionManager::GUIActionManager(GUI::Panel* base_panel)
    : _base_panel(base_panel)
{}

/* Set all visible actions to invisible */
void GUIActionManager::ClearVisibleActions()
{    
    for (auto& act : gui_actions) {
	this->RemoveAction(act.action->name.c_str());
	_base_panel->RemovePanel(act.btn);
	delete act.btn;
    }

    gui_actions.clear();
    is_dirty = true;
}

/* Add/replace action, and make it visible */
void GUIActionManager::AddVisibleAction(Action a)
{
    this->AddAction(a);
    auto* aptr = &(this->_actions[a.name]);
    SetActionVisible(aptr);
}

void GUIActionManager::SetActionVisible(Action* a)
{
    GUIAction guiact;
    guiact.action = a;

    int px, py, pw, ph;
    _base_panel->GetBounds(px, py, pw, ph);
    
    int x = lastx;
    int y = lasty;
  
    int w = default_width;
    int h = default_height;
    
    if (w > pw) {
	x = 0;
	y += default_height;
    }
    
    guiact.btn = new GUI::Button(x + default_margin, y + default_margin, w, h,
				 a->name.c_str());
    Action* act = guiact.action;
    guiact.btn->SetOnClickListener( [act, this](...) {
	    this->RunAction(act->name.c_str());
	});
    
    lastx = x + default_margin + w;
    lasty = y;
    gui_actions.push_back(guiact);
    is_dirty = true;
}

void GUIActionManager::SetVisibleActions(std::vector<const char*> actions)
{
    ClearVisibleActions();
    ActionManager::SetVisibleActions(actions);

    for (auto& actname : actions) {
	auto* aptr = &(this->_actions[actname]);
	SetActionVisible(aptr);
    }
}

void GUIActionManager::RemoveAction(const char* name)
{
    auto* aptr = &(this->_actions[name]);
    std::remove_if(gui_actions.begin(), gui_actions.end(), [&](GUIAction& a) {
	    return (a.action->name == aptr->name);
	});

    ActionManager::RemoveAction(name);
}

/* Update the 'base_panel' with all visible actions there */
void GUIActionManager::UpdateBasePanel()
{
    if (!is_dirty)
	return;
    
    _base_panel->ClearChildren();

    for (auto& f : this->gui_actions) {
	int bx, by, bw, bh;
	f.btn->GetBounds(bx, by, bw, bh);
	printf(" :: %s: %d %d %d %d\n", f.action->name.c_str(),
	       bx, by, bw, bh);
	
	_base_panel->AddPanel(f.btn);
    }
    is_dirty = false;
}

void GUIActionManager::RunAction(const char* name)
{
    for (auto& f : this->gui_actions) {
	if (!strcmp(name, f.action->name.c_str())) {
		ActionData ad;
		ad.xPos = 0;
		ad.yPos = 0;
		ad.actionOrigin = nullptr;

	    f.action->handler(f.action, ad);
	}
    }
}

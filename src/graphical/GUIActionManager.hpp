/*
  GUI action manager

  Helps binding interface actions to something graphical, something the user
  can click and see.

  Copyright (C) 2017, 2018 Arthur M
*/

#ifndef GUIACTIONMANAGER_HPP
#define GUIACTIONMANAGER_HPP


#include "../logic/ActionManager.hpp"
#include "gui/Panel.hpp"
#include "gui/Button.hpp"

#include <list>

namespace Tribalia::Graphics {

	struct GUIAction {
		Logic::Action* action;
		GUI::Button* btn;
	};

	class GUIActionManager : public Logic::ActionManager {
	private:
		GUI::Panel* _base_panel;

		std::list<GUIAction> gui_actions;

		int lastx = 0;
		int lasty = 0;
		const int default_width = 64;
		const int default_height = 64;
		const int default_margin = 8;

		bool is_dirty = false;

		void SetActionVisible(Logic::Action*);

	public:
		GUIActionManager(GUI::Panel* base_panel);

		/* Set all visible actions to invisible */
		void ClearVisibleActions();

		/* Add/replace action, and make it visible */
		void AddVisibleAction(Logic::Action);

		void SetVisibleActions(std::vector<const char*> actions);
		void RemoveAction(const char* name);


		/* Update the 'base_panel' with all visible actions there */
		void UpdateBasePanel();

		virtual void RunAction(const char* name) override;
	};
}


#endif //GUIACTIONMANAGER_HPP

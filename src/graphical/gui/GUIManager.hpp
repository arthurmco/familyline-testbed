/*
 * The one that runs the GUI rendering code
 *
 * Copyright (C) 2018 Arthur M
 */

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <vector>

#include "GUIControl.hpp"

namespace Familyline::Graphics::GUI {

    class GUIManager : public GUIControl {
    private:
	// Only force redraw on redraw events.
	bool force_redraw = false;

    public:
	std::vector<GUIControl*> controls;

	GUIManager();
	
	/** Add the controller (and send the containeradd event) **/
	void add(GUIControl* c);

	/**
	 * Try to handle the signal. Returns true if handled
	 */
	virtual bool processSignal(GUISignal s);

	// Process signals for all controls
	virtual void update();

    
	virtual void doRender(int absw, int absh) const;
    
	virtual void render(int absw, int absh);

    };

    
}


#endif /* GUIMANAGER_H */

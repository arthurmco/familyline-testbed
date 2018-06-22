/* Base class for all GUI controls
 *
 *  Copyright (C) 2018 Arthur Mendes
 */

#ifndef GUICONTROL_H
#define GUICONTROL_H

#include <queue>
#include <typeinfo>
#include <cairo/cairo.h>

#include "GUISignal.hpp"
#include <Log.hpp>

namespace Familyline::Graphics::GUI {

    /* The default type for GUI canvas */
    typedef cairo_surface_t* GUICanvas;

    /**
     * Base class for GUI controls
     *
     * Defines just the base for our control to be identifiable (and maybe renderable? )
     */
    class GUIControl {
    protected:
	// How to draw
	GUICanvas canvas;
	bool dirty = true;

	std::queue<GUISignal> signals;

	GUISignal receiveSignal();

	bool hasSignal();

	/* The function that does the real render
	   It needs to be pure.
	   If you want to do impure things, override the render function,
	   or simply don't do them here.
	*/
	virtual void doRender(int absw, int absh) const = 0;

    public:
	// Positional
	float width, height;
	float x, y;

	/**
	 * Try to handle the signals. Returns true if handled
	 */
	virtual bool processSignal(GUISignal s);

	virtual void update();

	/* Start the rendering process.
	   Just calls doRender() and set the dirty status to false.
	   This function only exists so we can call render() inside tests.
	*/
	void render(int absw, int absh);

	virtual ~GUIControl() {}

	bool isDirty() const;

	GUICanvas getGUICanvas() const;

	void sendSignal(GUISignal s);

    };


}


#endif /* GUICONTROL_H */

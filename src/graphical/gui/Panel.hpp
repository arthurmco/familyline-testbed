/*  Panel rendering class

    Copyright (C) 2016 Arthur M
*/

#include <cairo/cairo.h>
#include <vector>

#include "IContainer.hpp"

#ifndef _GUI_PANEL
#define _GUI_PANEL

namespace Tribalia {
namespace Graphics {
namespace GUI {

    struct PanelData {
	IPanel* panel;
	int x, y, w, h;

	cairo_t* ctxt;
	cairo_surface_t* csurf;
    };
    
/*
    A panel is the most basic window structore for Tribalia's GUI
*/
class Panel : public IContainer {
private:
        
    std::vector<PanelData> _panels;
public:
    Panel();
    Panel(int x, int y, int w, int h);

    virtual void Redraw(cairo_t* ctxt) override;

    /* Add a panel using the panel position or a new position */
    virtual int AddPanel(IPanel* p) override;
    virtual int AddPanel(IPanel* p, int x, int y) override;

    /* Remove the panel */
    virtual void RemovePanel(IPanel* p) override;

    virtual bool ProcessInput(Input::InputEvent& ev) override;

};

}
}
}


#endif /* end of include guard: _GUI_WINDOW */

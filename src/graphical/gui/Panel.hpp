/*  Panel rendering class

    Copyright (C) 2016 Arthur M
*/

#include <cairo/cairo.h>
#include <vector>

#include "IPanel.hpp"

#ifndef _GUI_PANEL
#define _GUI_PANEL

namespace Tribalia {
namespace Graphics {
namespace GUI {

/*
    A panel is the most basic window structore for Tribalia's GUI
*/
class Panel : public IPanel {
private:
    int _xPos, _yPos;
    int _width, _height;

    std::vector<Panel*> _panels;
public:
    Panel();
    Panel(int x, int y, int w, int h);

    virtual void SetBounds(int x, int y, int w, int h) override;
    virtual void SetPosition(int x, int y) override;
    virtual void GetBounds(int& x, int& y, int& w, int& h) override;

    virtual void Redraw() override;

    /* Add a panel using the panel position or a new position */
    virtual int AddPanel(IPanel* p) override;
    virtual int AddPanel(IPanel* p, int x, int y) override;

    /* Remove the panel */
    virtual void RemovePanel(IPanel* p) override;


};

}
}
}


#endif /* end of include guard: _GUI_WINDOW */

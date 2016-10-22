/*  Interface for a panel

    Copyright (C) 2016 Arthur M
*/

#include "ControlUpdater.hpp"

#ifndef _GUI_IPANEL
#define _GUI_IPANEL

namespace Tribalia {
namespace Graphics {
namespace GUI {

/*
    Interface for implementing a panel

    This is here to ease treating GUIRenderer like a panel, so we can
    easily implement redrawing and adding/deletion of controls without
    circular references
*/
class IPanel : public IControlUpdater {

public:
    /* Add a panel using the panel position or a new position */
    virtual int AddPanel(IPanel* p) = 0;
    virtual int AddPanel(IPanel* p, int x, int y) = 0;

    /* Remove the panel */
    virtual void RemovePanel(IPanel* p) = 0;

    virtual void SetBounds(int x, int y, int w, int h) = 0;
    virtual void SetPosition(int x, int y) = 0;
    virtual void GetBounds(int& x, int& y, int& w, int& h) = 0;
};

}
}
}


#endif /* end of include guard: _GUI_WINDOW */

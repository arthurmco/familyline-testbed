/*
    Container class

    Copyright (C) 2016 Arthur M
*/

#include "IPanel.hpp"

#ifndef _GUI_ICONTAINER_HPP
#define _GUI_ICONTAINER_HPP

namespace Tribalia {
namespace Graphics {
namespace GUI {

/*
    A container is a control that allows subcontrols
*/

class IContainer : public IPanel
{    
public:
    /* Add a panel using the panel position or a new position */
    virtual int AddPanel(IPanel* p) = 0;
    virtual int AddPanel(IPanel* p, int x, int y) = 0; // add in absolute pos
    virtual int AddPanel(IPanel* p, double x, double y) = 0; // add in relative pos

    /* Remove the panel */
    virtual void RemovePanel(IPanel* p) = 0;

    virtual ~IContainer(){}
};

}
}
}


#endif /* end of include guard: _GUI_ICONTAINER_HPP */

/*  Interface for a panel

    Copyright (C) 2016 Arthur M
*/

#include "IControl.hpp"
#include <cstdint>

#ifndef _GUI_IPANEL
#define _GUI_IPANEL

namespace Tribalia {
namespace Graphics {
namespace GUI {

/*
    Interface for implementing a panel

    This is here to ease treating GUIRenderer like a panel, so we can
    easily implement redrawing without
    circular references

    Panels usually have positions and sizes
*/
class IPanel : public IControl {
protected:
    int _bgColor;  /* control background color, in RGBA format */
    int _xPos, _yPos;
    int _width, _height;

public:
    IPanel();
    IPanel(int x, int y, int w, int h);

    void SetBounds(int x, int y, int w, int h);
    void SetPosition(int x, int y);
    void GetBounds(int& x, int& y, int& w, int& h);

    /*  Non-virtual functions in an interface?
        C++ allows this. I won't implement this on every control I make. */
    int GetBackColor();
    int GetBackColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a);
    void SetBackColor(int rgba);
    void SetBackColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a);
};

}
}
}


#endif /* end of include guard: _GUI_WINDOW */

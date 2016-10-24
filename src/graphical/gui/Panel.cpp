#include "Panel.hpp"

using namespace Tribalia::Graphics::GUI;

Panel::Panel() : _xPos(0), _yPos(0), _width(0), _height(0)
{
    _bgColor = 0x778899cc;
}

Panel::Panel(int x, int y, int w, int h)
    : _xPos(x), _yPos(y), _width(w), _height(h)
{
     _bgColor = 0x778899cc;
}

void Panel::SetBounds(int x, int y, int w, int h)
{
    _xPos = x;
    _yPos = y;
    _width = w;
    _height = h;
}

void Panel::SetPosition(int x, int y)
{
    _xPos = x;
    _yPos = y;
}

void Panel::GetBounds(int& x, int& y, int& w, int& h)
{
    x = _xPos;
    y = _yPos;
    w = _width;
    h = _height;
}

void Panel::Redraw()
{
    for (Panel* p : _panels) {
        p->Redraw();

    }
}

/* Add a panel using the panel position or a new position */
int Panel::AddPanel(IPanel* p)
{
    _panels.push_back((Panel*)p);
    return 1;
}
int Panel::AddPanel(IPanel* p, int x, int y)
{
    Panel* _p = (Panel*)p;
    _p->SetPosition(x, y);
    _panels.push_back(_p);
    return 1;
}

/* Remove the panel */
void Panel::RemovePanel(IPanel* p)
{

}

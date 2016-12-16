#include "Panel.hpp"

using namespace Tribalia::Graphics::GUI;

Panel::Panel()
{
    _bgColor = 0x778899cc;
}

Panel::Panel(int x, int y, int w, int h)
{
    SetBounds(x,y,w,h);
     _bgColor = 0x778899cc;
}

void Panel::Redraw(cairo_t* ctxt)
{
    for (Panel* p : _panels) {
        p->Redraw(ctxt);

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

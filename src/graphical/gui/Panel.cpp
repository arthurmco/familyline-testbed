#include "Panel.hpp"

using namespace Tribalia::Graphics::GUI;

Panel::Panel()
{
    _bgColor = glm::vec4(0,0,0,0);
}

Panel::Panel(int x, int y, int w, int h)
    : Panel()
{
    SetBounds(x,y,w,h);
}

void Panel::Redraw(cairo_t* ctxt)
{
    /* Paint background */
    cairo_set_source_rgba(ctxt, _bgColor.r, _bgColor.g, _bgColor.b, _bgColor.a);
    cairo_set_operator(ctxt, CAIRO_OPERATOR_SOURCE);
    cairo_paint(ctxt);
    
    for (auto& p : _panels) {
	
	/* Redraw other panels, then put the panel data here */
	p.panel->Redraw(p.ctxt);

	cairo_set_operator(ctxt, CAIRO_OPERATOR_OVER);
	cairo_set_source_surface(ctxt, p.csurf, p.x, p.y);
	cairo_paint(ctxt);

	cairo_save(p.ctxt);
	cairo_set_source_rgba(p.ctxt, 0.0, 0.0, 0.0, 0.0);
	cairo_set_operator(p.ctxt, CAIRO_OPERATOR_SOURCE);
	cairo_paint(p.ctxt);
	cairo_restore(p.ctxt); 
	
    }

}

/* Add a panel using the panel position or a new position */
int Panel::AddPanel(IPanel* p)
{
    PanelData pd;
    pd.panel = p;

    p->GetBounds(pd.x, pd.y, pd.w, pd.h);

    pd.csurf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, pd.w, pd.h);
    pd.ctxt = cairo_create(pd.csurf);
    
    _panels.push_back(pd);
    return 1;
}
int Panel::AddPanel(IPanel* p, int x, int y)
{
    Panel* _p = (Panel*)p;
    _p->SetPosition(x, y);
    return this->AddPanel(p);
}

/* Remove the panel */
void Panel::RemovePanel(IPanel* p)
{

}

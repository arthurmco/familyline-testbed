#include "Button.hpp"
#include "Label.hpp"

using namespace Tribalia::Graphics::GUI;
using namespace Tribalia;

Button::Button(int x, int y, int w, int h, const char* text)
    : IPanel(int(x),int(y),w,h)
{
    panel_data.panel = new Label{0, 0, text};
    _bgColor = glm::vec4(0.5, 0.5, 0.5, 1);
    _fgColor = glm::vec4(0.85, 0, 0, 1);
    _actualbg = _bgColor;

}

Button::Button(double x, double y, double w, double h, const char* text)
    : IPanel(double(x),double(y),w,h, true)
{
    panel_data.panel = new Label{0, 0, text};
    _bgColor = glm::vec4(0.5, 0.5, 0.5, 1);
    _fgColor = glm::vec4(0.85, 0, 0, 1);
    _actualbg = _bgColor;    
}


const IPanel* Button::GetPanel()
{
    panel_changed = panel_data.panel->IsDirty();
    return panel_data.panel;
}

void Button::Redraw(cairo_t* ctxt)
{
    _dirty = (!panel_data.panel_ctxt || isClick || isHover);
    
    int offsx = (_width-4)/2 - ( panel_data.panel->GetDataWidth() / 2 );
    int offsy = (_height-4)/2 - ( panel_data.panel->GetDataHeight());
    
    if (!panel_data.panel_ctxt) {
	panel_data.panel_surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
							   _width, _height);
	panel_data.panel_ctxt = cairo_create(panel_data.panel_surf);

    }
    
    if (panel_data.panel->IsDirty()) {
	cairo_save(panel_data.panel_ctxt);
	cairo_set_source_rgba(panel_data.panel_ctxt, 0.0, 0.0, 0.0, 0.0);
	cairo_set_operator(panel_data.panel_ctxt, CAIRO_OPERATOR_SOURCE);
	cairo_paint(panel_data.panel_ctxt);
	cairo_restore(panel_data.panel_ctxt); 

	panel_data.panel->Redraw(panel_data.panel_ctxt);
    }

    if (!isClick && !isHover) 
	_actualbg = _bgColor;
        
    /* Fill square */
    cairo_rectangle(ctxt, 0, 0, _width, _height);
    cairo_set_source_rgba(ctxt, _actualbg.r, _actualbg.g,
			  _actualbg.b, _actualbg.a);
    cairo_fill(ctxt);


    /* Draw square */
    cairo_rectangle(ctxt, 2, 2, _width-4, _height-4);
    glm::vec4 bstroke = glm::vec4(_actualbg.r / 1.41, _actualbg.g / 1.41,
				  _actualbg.b / 1.41, _actualbg.a);
    
    cairo_set_source_rgba(ctxt, bstroke.r, bstroke.g, bstroke.b,
			  bstroke.a);
    cairo_set_line_width(ctxt, 4.0);
    cairo_stroke(ctxt);

    /* Paint the other object over */
    cairo_set_source_surface(ctxt, panel_data.panel_surf, 4+offsx, offsy);
    cairo_paint(ctxt);

    _dirty = (false);
    
}

bool Button::ProcessInput(Input::InputEvent& ev)
{
    isInput = true;
    glm::vec4 bgclick = glm::vec4(_bgColor.r / 1.71f, _bgColor.g / 1.71f,
				  _bgColor.b / 1.71f, _bgColor.a * 1.71f);
    glm::vec4 bghover = glm::vec4(_bgColor.r / 1.41f, _bgColor.g / 1.41f,
				  _bgColor.b / 1.41f, _bgColor.a * 1.41f);

    _dirty = true;
    switch (ev.eventType) {
    case Input::EVENT_MOUSEEVENT:
	if (ev.event.mouseev.status != Input::KEY_KEYRELEASE) {
	    _actualbg = bgclick;

	    if (onClickListener && !isClick ) {
		this->onClickListener(this);
	    }

	    isClick = true;
	    _dirty = true;
	} else {
	    isClick = false;
	    _dirty = true;
	    _actualbg = bghover;
	}
	break;
    case Input::EVENT_MOUSEMOVE:
	if (isHover) {
	    _actualbg = bghover;
	}
	break;
    }

    return true;
}

void Button::OnFocus()
{
    isHover = true;
    printf("Focus in\n");
    _dirty = true;
}

void Button::OnLostFocus()
{
    isHover = false;
    printf("Focus out\n");
    _actualbg = _bgColor;
    _dirty = true;
}

void Button::SetOnClickListener(OnClickListener ocl)
{
    if (ocl)
	onClickListener = ocl;
}

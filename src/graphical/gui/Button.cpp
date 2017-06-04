#include "Button.hpp"

using namespace Tribalia::Graphics::GUI;
using namespace Tribalia;

Button::Button(int x, int y, int w, int h, const char* text)
    : IPanel(int(x),int(y),w,h)
{
    _text = std::string{text};
    _bgColor = glm::vec4(128, 128, 128, 255);
    _fgColor = glm::vec4(32, 0, 0, 255);
    _actualbg = _bgColor;
}

Button::Button(double x, double y, double w, double h, const char* text)
    : IPanel(double(x),double(y),w,h, true)
{
    printf("t: %s\n", text);
    _text = std::string{text};
    _bgColor = glm::vec4(128, 128, 128, 255);
    _fgColor = glm::vec4(32, 0, 0, 255);
    _actualbg = _bgColor;
}


const char* Button::GetText() const
{
    return _text.c_str();
}

void Button::SetText(char* txt, ...)
{
    va_list vl;
    va_start(vl, txt);
    char* s = new char[512+strlen(txt)];
    vsprintf(s, txt, vl);
    va_end(vl);

    textChanged = true;
    _text = std::string{s};
}


void Button::Redraw(cairo_t* ctxt)
{
    /* Fill square */
    cairo_rectangle(ctxt, 0, 0, _width, _height);
    cairo_set_source_rgba(ctxt, _actualbg.r/255, _actualbg.g/255,
			  _actualbg.b/255, _actualbg.a/255);
    cairo_fill(ctxt);


    
    /* Draw square */
    cairo_rectangle(ctxt, 2, 2, _width-4, _height-4);
    glm::vec4 bstroke = glm::vec4(_actualbg.r / 1.41, _actualbg.g / 1.41,
				  _actualbg.b / 1.41, _actualbg.a);
    
    cairo_set_source_rgba(ctxt, bstroke.r/255, bstroke.g/255, bstroke.b/255,
			  bstroke.a/255);
    cairo_set_line_width(ctxt, 4.0);
    cairo_stroke(ctxt);
    

    /* If text changed, remeasure it, so the label will be in the
       exact center of the button */
    if (textChanged) {
	cairo_text_extents(ctxt, _text.c_str(), &extents);	
	textChanged = false;
	printf("%.2f x %.2f\n", extents.width, extents.height);
    }

    /* Draw text */
    cairo_set_source_rgba(ctxt, _fgColor.r, _fgColor.g, _fgColor.b, _fgColor.a);
    cairo_select_font_face(ctxt, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(ctxt, 14.0);
    cairo_move_to(ctxt, (_width-6)/2.0 - (extents.width)/2.0,
		  (_height)/2.0 + (extents.height) - (extents.height)/2.0);
		  
		  
    cairo_show_text(ctxt, _text.c_str());
    
}

bool Button::ProcessInput(Input::InputEvent& ev)
{
    isInput = true;
    glm::vec4 bgclick = glm::vec4(_bgColor.r / 1.71f, _bgColor.g / 1.71f,
				  _bgColor.b / 1.71f, _bgColor.a * 1.71f);
    glm::vec4 bghover = glm::vec4(_bgColor.r / 1.41f, _bgColor.g / 1.41f,
				  _bgColor.b / 1.41f, _bgColor.a * 1.41f);
    
    switch (ev.eventType) {
    case Input::EVENT_MOUSEEVENT:
	if (ev.event.mouseev.status != Input::KEY_KEYRELEASE) {
	    _actualbg = bgclick;

	    if (onClickListener)
		this->onClickListener(this);
	} else {
	    _actualbg = bghover;
	}
	break;
    case Input::EVENT_MOUSEMOVE:
	if (isHover)
	    _actualbg = bghover;
	break;
    }
    
    return true;
}

void Button::OnFocus()
{
    isHover = true;
    printf("Focus in\n");
}

void Button::OnLostFocus()
{
    isHover = false;
    printf("Focus out\n");
    _actualbg = _bgColor;
}

void Button::SetOnClickListener(OnClickListener ocl)
{
    if (ocl)
	onClickListener = ocl;
}

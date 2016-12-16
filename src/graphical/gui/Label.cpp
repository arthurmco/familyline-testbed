#include "Label.hpp"

using namespace Tribalia::Graphics::GUI;

Label::Label(int x, int y, const char* text)
{
    _text = std::string{text};
    _xPos = x;
    _yPos = y;
    _width = _text.size() * 9;
    _height = 10;
}

Label::Label(int x, int y, int w, int h, const char* text)
    : IPanel(x,y,w,h)
{
    _text = std::string{text};
}

const char* Label::GetText() const
{
    return _text.c_str();
}

void Label::SetText(char* txt)
{
    _text = std::string{txt};
}

void Label::Redraw(cairo_t* ctxt)
{
    /* Redraw the text */
    cairo_set_source_rgba(ctxt, 1.0, 0.0, 0.0, 1.0);
	cairo_select_font_face(ctxt, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(ctxt, 14.0);
    cairo_move_to(ctxt, _xPos*1.0, (_yPos*1.0)+_height-1.0);
    cairo_show_text(ctxt, _text.c_str());
}
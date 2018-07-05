#include "GUILabel.hpp"

using namespace Familyline::Graphics::GUI;


GUILabel::GUILabel(float x, float y, const char* text)
    : text(text)
{
    this->x = x;
    this->y = y;
    this->width = 0.5;
    this->height = 0.5;

}



// Set the label size according to the text
void GUILabel::autoResize(GUISignal s)
{

    // Calculate the size of the rendered text. This will be the width
    cairo_text_extents_t te;
    cairo_text_extents (ctxt, this->text.c_str(), &te);

    this->width = this->text.size() * 10.0 / s.absw;
    
    this->height = (16.0+8.0) / s.absh;
}


bool GUILabel::processSignal(GUISignal s)
{
    switch (s.signal) {
    case ContainerAdd:

	if (this->width < 0) {
	    this->width = 0.1;
	    this->height = 0.1;

	}

	return true;

    case Redraw:
	GUIControl::processSignal(s);

	this->autoResize(s);
	GUIControl::processSignal(s);

	return true;

    default:
	return GUIControl::processSignal(s);

    }
}

const char* GUILabel::getText() const { return text.c_str(); }
void GUILabel::setText(const char* s)
{
    text = std::string{s};
    this->dirty = true;
}


GUICanvas GUILabel::doRender(int absw, int absh) const
{
    
    cairo_set_source_rgb(ctxt, 1, 0.5, 0);
    cairo_rectangle(ctxt, 0, 0, width*absw, height*absh);
    cairo_stroke(ctxt);

    
    cairo_set_source_rgb (ctxt, 0.0, 1.0, 0.0);
    cairo_set_font_size(ctxt, 16);
    cairo_select_font_face (ctxt, "Arial",
			    CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_text_extents_t  te;
    cairo_text_extents (ctxt, this->text.c_str(), &te);
    cairo_move_to(ctxt, 0, 16);
    cairo_show_text (ctxt, this->text.c_str());

    return this->canvas;
}

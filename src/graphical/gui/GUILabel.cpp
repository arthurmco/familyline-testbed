#include "GUILabel.hpp"
#include <cstring> // for strcmp

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

    this->width = (te.width + te.x_advance) / (s.absw * 1.25);

    this->height = (te.height - te.y_bearing) / (s.absh * 0.75);
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

	this->absw = s.absw;
	this->absh = s.absh;
	    
	
	return true;

    default:
	return GUIControl::processSignal(s);

    }
}

const char* GUILabel::getText() const { return text.c_str(); }
void GUILabel::setText(const char* s)
{

    // Only update if text is different
    if (strncmp(s, text.c_str(), text.size()+1)) {
	text = std::string{s};

	this->dirty = true;
	
	GUISignal sig = {};
	sig.signal = SignalType::Redraw;
	sig.absw = this->absw;
	sig.absh = this->absh;

	this->autoResize(sig);
	this->setContext(this->absw, this->absh);

	fprintf(stderr, ">>> %s %.2f %.2f %.3f %.3f\n", s, sig.absw, sig.absh, this->width, this->height);
//    fprintf(stderr, "%.2f %.2f <<<%s>>> %.3f %.3f \n", sig.absw, sig.absh, s, this->width, this->height);
    }
}


GUICanvas GUILabel::doRender(int absw, int absh) const
{
    
    cairo_set_source_rgb (ctxt, 0.4, 0.4, 0.4);
    cairo_set_font_size(ctxt, 16);
    cairo_select_font_face (ctxt, "Arial",
			    CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_text_extents_t  te;
    cairo_text_extents (ctxt, this->text.c_str(), &te);
    cairo_move_to(ctxt, 0, 16);
    cairo_show_text (ctxt, this->text.c_str());

    return this->canvas;
}

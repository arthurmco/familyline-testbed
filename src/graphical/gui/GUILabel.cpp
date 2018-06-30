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
	    
    this->width = float(te.width / s.absw);
    this->height = float(te.height / s.absh);
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
    
    
GUICanvas GUILabel::doRender(int absw, int absh) const {
    (void)absw;
    (void)absh;

    cairo_set_source_rgb (ctxt, 0.0, 0.0, 0.0);
    cairo_set_font_size(ctxt, 16);
    cairo_select_font_face (ctxt, "Arial",
			    CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_text_extents_t te;
    cairo_text_extents (ctxt, this->text.c_str(), &te);
    cairo_move_to (ctxt, te.width / 8 - te.x_bearing,
		   te.height / 2 - te.y_bearing);
    
    cairo_show_text (ctxt, this->text.c_str());
    
    return this->canvas;
}

GUILabel::~GUILabel()
{
//    delete[] this->canvas;
}


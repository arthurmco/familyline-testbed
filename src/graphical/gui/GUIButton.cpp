#include "GUIButton.hpp"

using namespace Familyline::Graphics::GUI;

GUIButton::GUIButton(float x, float y, float w, float h, const char* text)
{
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;

    // add a dummy handler
    this->onClickHandler = [](GUIControl* c){(void)c;};

    this->label = new GUILabel(x+(w*0.4), y+(h*0.2), text);
}

bool GUIButton::processSignal(GUISignal s) {
    switch (s.signal) {
    case SignalType::MouseClick:
	this->onClickHandler(this);
	return true;
    case SignalType::Redraw:
	label->setContext(s.absw, s.absh);
    default:
	return GUIControl::processSignal(s);
	
    }
}

GUICanvas GUIButton::doRender(int absw, int absh) const {
    auto canvas_label = label->doRender(absw, absh);
    
    cairo_set_source_rgb(ctxt, 1, 1, 1);
    cairo_paint(ctxt);
    
    cairo_set_line_width(ctxt, 5);
    cairo_set_source_rgb(ctxt, 1, 0, 0);
    cairo_rectangle(ctxt, 0, 0, width*absw, height*absh);
    cairo_stroke(ctxt);

    cairo_set_operator(ctxt, CAIRO_OPERATOR_OVER);
    cairo_set_source_surface(ctxt, canvas_label, 0, 0);
    cairo_paint(ctxt);

    return this->canvas;
}

GUIButton::~GUIButton() {
    //   delete[] this->canvas;
    delete label;
}

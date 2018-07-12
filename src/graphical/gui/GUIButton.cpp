#include "GUIButton.hpp"
#include <Log.hpp>

using namespace Familyline;
using namespace Familyline::Graphics::GUI;

GUIButton::GUIButton(float x, float y, float w, float h, const char* text)
{
	this->x = x;
	this->y = y;
	this->width = w;
	this->height = h;

	// add a dummy handler
	this->onClickHandler = [](GUIControl* c) {(void)c; };

	this->format.foreground = glm::vec4(1, 0, 0, 0);
	this->format.background = glm::vec4(1, 1, 1, 0.4);

	this->label = new GUILabel(x + (w*0.4), y + (h*0.2), text);
	this->label->format.fontSize = 18;
}

bool GUIButton::processSignal(GUISignal s) {

	Log::GetLog()->InfoWrite("gui-control", "Received signal %#x from %s (%p) to button '%s' "
		"(%p), %.2f x %.2f",
		s.signal, (s.from ? typeid(*s.from).name() : "(null)"),
		s.from, this->label->getText(), s.to, s.xPos, s.yPos);


	switch (s.signal) {
	case SignalType::MouseClick:
		fprintf(stderr, "!");

		this->onClickHandler(this);
		return true;
	case SignalType::Redraw:
		label->format = this->format;
		label->setContext(s.absw, s.absh);
		label->processSignal(s);
	default:
		return GUIControl::processSignal(s);

	}
}

GUICanvas GUIButton::doRender(int absw, int absh) const {
	auto canvas_label = label->doRender(absw, absh);
	const auto labelw = this->label->width*absw;

	cairo_set_source_rgb(ctxt, format.background.r, format.background.g, format.background.b);
	cairo_paint(ctxt);

	cairo_set_line_width(ctxt, 5);
	cairo_set_source_rgb(ctxt, format.foreground.r, format.foreground.g, format.foreground.b);
	cairo_rectangle(ctxt, 0, 0, width*absw, height*absh);
	cairo_stroke(ctxt);

	cairo_set_operator(ctxt, CAIRO_OPERATOR_OVER);
	cairo_move_to(ctxt, 0, 0);
	cairo_set_source_surface(ctxt, canvas_label, this->width*absw / 2 - labelw / 2, this->height / 2);
	cairo_paint(ctxt);

	return this->canvas;
}

GUIButton::~GUIButton() {
	//   delete[] this->canvas;
	delete label;
}

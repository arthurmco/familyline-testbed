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
    return label->doRender(absw, absh);

}

GUIButton::~GUIButton() {
    //   delete[] this->canvas;
    delete label;
}

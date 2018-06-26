#include "GUILabel.hpp"

using namespace Familyline::Graphics::GUI;


GUILabel::GUILabel(float x, float y, const char* text)
    : text(text)
{
    this->x = x;
    this->y = y;
    this->width = -1;
//    this->canvas = new char[1024];
}

bool GUILabel::processSignal(GUISignal s)
{
    switch (s.signal) {
    case ContainerAdd:

	// TODO: move this to processsignal
	if (this->width < 0) {
	    this->width = float(text.size()) / s.absw;
	    this->height = 2.0 / s.absh;
	}

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
    
    
void GUILabel::doRender(int absw, int absh) const {
    (void)absw;
    (void)absh;
    
}

GUILabel::~GUILabel()
{
//    delete[] this->canvas;
}


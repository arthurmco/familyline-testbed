#include "GUIControl.hpp"
#include <Log.hpp>

using namespace Familyline::Graphics::GUI;

GUISignal GUIControl::receiveSignal()
{
    auto s = signals.front();
    signals.pop();
    Log::GetLog()->InfoWrite("gui-control", "Received signal %#x from %s (%p) to %s (%p), %.2f x %.2f",
			     s.signal, (s.from ? typeid(*s.from).name() : "(null)"),
			     s.from, typeid(*s.to).name(), s.to, s.xPos, s.yPos);


    return GUISignal(s);
}

bool GUIControl::hasSignal() { return !signals.empty(); }

/* Update the drawing context.
 *
 * This is design under Cairo API. The context (cairo_t) is used for drawing, and
 * it can only be created by the cairo_create. The cairo_create takes a surface
 * (the GUICanvas, cairo_surface_t). This means a context is binded to a surface
 *
 * absw and absh are the absolute width and height of the whole window, not the control
 */
void GUIControl::setContext(unsigned absw, unsigned absh)
{
    if (this->ctxt)
		cairo_destroy(this->ctxt);

    if (this->canvas)
		cairo_surface_destroy(this->canvas);
    
    this->canvas = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, this->width*absw, this->height*absh);
    this->ctxt = cairo_create(this->canvas);
}


/**
 * Try to handle the signals. Returns true if handled
 */
bool GUIControl::processSignal(GUISignal s)
{
    //Only natively process the redraw signal */
    if (s.signal == SignalType::Redraw) {
	this->dirty = true;
	this->setContext(s.absw, s.absh);
	return true;
    }

    return false;
}

void GUIControl::update()
{
    while (this->hasSignal()) {
	auto s = this->receiveSignal();
	this->processSignal(s);
    }
}

/* Start the rendering process.
   Just calls doRender() and set the dirty status to false.
   This function only exists so we can call render() inside tests.
*/
void GUIControl::render(int absw, int absh)
{
    this->canvas = this->doRender(absw, absh);
    this->dirty = false;
}

bool GUIControl::isDirty() const { return this->dirty; }

GUICanvas GUIControl::getGUICanvas() const { return this->canvas; }

void GUIControl::sendSignal(GUISignal s)
{
    signals.push(s);
}

GUIControl::~GUIControl()
{
   
    if (this->canvas)
	cairo_surface_destroy(this->canvas);
}

#include "GUIControl.hpp"

using namespace Familyline::Graphics::GUI;

GUISignal GUIControl::receiveSignal()
{
    auto s = signals.front();
    signals.pop();
    Log::GetLog()->InfoWrite("gui-control", "Received signal %#x from %s (%p) to %s (%p), %.2f x %.2f\n",
			     s.signal, (s.from ? typeid(*s.from).name() : "(null)"),
			     s.from, typeid(*s.to).name(), s.to, s.xPos, s.yPos);


    return GUISignal(s);
}

bool GUIControl::hasSignal() { return !signals.empty(); }

/**
 * Try to handle the signals. Returns true if handled
 */
bool GUIControl::processSignal(GUISignal s)
{
    //Only natively process the redraw signal */
    if (s.signal == SignalType::Redraw) {
	this->dirty = true;
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
    this->doRender(absw, absh);
    this->dirty = false;
}

bool GUIControl::isDirty() const { return this->dirty; }

GUICanvas GUIControl::getGUICanvas() const { return this->canvas; }

void GUIControl::sendSignal(GUISignal s)
{
    signals.push(s);
}

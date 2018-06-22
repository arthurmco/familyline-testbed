#include "GUIManager.hpp"

using namespace Familyline::Graphics::GUI;

GUIManager::GUIManager()
{
    x = 0;
    y = 0;

    width = float(640);
    height = float(480);
}

/** Add the controller (and send the containeradd event) **/
void GUIManager::add(GUIControl* c)
{
    auto caddsig = GUISignal{this, c, SignalType::ContainerAdd, 0, 0};
    caddsig.absw = this->width;
    caddsig.absh = this->height;
	
    c->sendSignal(caddsig);
    this->controls.push_back(c);
}

/**
 * Try to handle the signal. Returns true if handled
 */
bool GUIManager::processSignal(GUISignal s)
{
    //Only natively process the redraw signal */
    if (s.signal == SignalType::Redraw) {
	// We redraw this by getting the width and height again

	width = float(640);
	height = float(480);

	this->dirty = true;
	this->force_redraw = true;
	return true;
    } else {

	/* Pass the rest of  signals to its children */
	GUISignal ns = s;
	ns.from = this;


	for (auto* control : controls) {
	    if (s.xPos >= control->x && s.xPos <= (control->x + control->width) &&
		s.yPos >= control->y && s.xPos <= (control->y + control->height) ) {
		ns.to = control;
		// Pass relative positions
		// TODO: Pass absolute positions somehow.
		// (maybe not, with SignalType::ContainerAdd this might not be needed)
		ns.xPos = s.xPos - control->x;
		ns.yPos = s.yPos - control->y;
		control->sendSignal(ns);
	    }
	}

    }

    return true;
}

// Process signals for all controls
void GUIManager::update()
{
    GUIControl::update();
    for (auto* control : controls) {
	control->update();
    }
}

    
void GUIManager::doRender(int absw, int absh) const
{
    for (auto* control : controls) {
	if (control->isDirty() || this->force_redraw)
	    control->render(this->width, this->height);

	printf("%s", control->getGUICanvas());
    }
}
    
void GUIManager::render(int absw, int absh)
{

    this->doRender(absw, absh);
    this->force_redraw = false;
}

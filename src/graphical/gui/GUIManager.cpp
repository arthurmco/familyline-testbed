#include "GUIManager.hpp"

using namespace Familyline::Graphics::GUI;
using namespace Familyline::Input;

GUIManager::GUIManager()
{
    x = 0;
    y = 0;

    width = float(640);
    height = float(480);

    this->listener = new Input::InputListener("gui-listener");
    Input::InputManager::GetInstance()->AddListener(
	Input::EVENT_KEYEVENT | Input::EVENT_MOUSEMOVE | Input::EVENT_MOUSEEVENT,
	this->listener, 0.8);

    // Create the cairo context.
    // It wiil do this->width * 1
    this->setContext(1, 1);
}

/** Add the controller (and send the containeradd event) **/
void GUIManager::add(GUIControl* c)
{
    auto caddsig = GUISignal{this, c, SignalType::ContainerAdd, 0, 0};
    caddsig.absw = this->width;
    caddsig.absh = this->height;
    c->sendSignal(caddsig);

    caddsig = GUISignal{this, c, SignalType::Redraw, 0, 0};
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
		s.yPos >= control->y && s.yPos <= (control->y + control->height) ) {
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
    // Receive the input signals from the input listeners...
    Input::InputEvent ev;
    while (this->listener->PopEvent(ev)) {
	SignalType signalType;
	GUISignal gs;
	
	switch (ev.eventType) {
	case EVENT_MOUSEMOVE: signalType = SignalType::MouseHover; break;
	case EVENT_MOUSEEVENT:
	    signalType = SignalType::MouseClick;

	    gs.mouse.button = ev.event.mouseev.button;
	    gs.mouse.isPressed = ev.event.mouseev.status == KEY_KEYPRESS;
	    
	    break;
	default: continue; // Ignore other events for now
	}
	
	gs.xPos = ev.mousex / this->width;
	gs.yPos = ev.mousey / this->height;
	gs.from = nullptr;
	gs.to = this;
	gs.signal = signalType;
	this->sendSignal(gs);
    }

    // ...and send them to the controls
    GUIControl::update();
    for (auto* control : controls) {
	control->update();
    }
}

    
GUICanvas GUIManager::doRender(int absw, int absh) const
{
    (void)absw;
    (void)absh;

    // Clean bg
    cairo_set_source_rgba(ctxt, 0, 0, 0, 0);
    cairo_set_operator(ctxt, CAIRO_OPERATOR_SOURCE);
    cairo_paint(ctxt);
    
    for (auto* control : controls) {
	if (control->isDirty() || this->force_redraw)
	    control->render(this->width, this->height);

	unsigned absx = control->x * absw;
	unsigned absy = control->y * absh;
	
	auto ccanvas = control->getGUICanvas();
	cairo_set_operator(ctxt, CAIRO_OPERATOR_OVER);
	cairo_set_source_surface(ctxt, ccanvas, absx, absy);
	cairo_paint(ctxt);
	
    }

    return this->canvas;
}
    
void GUIManager::render(int absw, int absh)
{
    unsigned long long stack_bs;
    unsigned long long stack_b2ss;
    auto c = this->doRender(absw, absh);

    char s[64];
    sprintf(s, "%llx_%llx.png", stack_bs, stack_b2ss);
    cairo_surface_write_to_png(c, s);
    
    this->force_redraw = false;
}

#include "GUIManager.hpp"
#include "../../config.h" //for SHADERS_DIR

using namespace Familyline::Graphics;
using namespace Familyline::Graphics::GUI;
using namespace Familyline::Input;

/* The panel vertex square coordinates.
   It's a big rectangle, that fills the entire screen  */
static const float window_pos_coord[][3] =
{
    {-1, 1, 1}, {-1, -1, 1}, {1, -1, 1},
    {-1, 1, 1}, {1, 1, 1}, {1, -1, 1}

    // {-1, -1, 1}, { 1, -1, 1}, { 1, 1, 1},
    // {-1, -1, 1}, {-1,  1, 1}, { 1, 1, 1}
};

/* Coordinates for every panel texture.
   Since they have the same vertex order, we don't need to declare multiple
   texture coordinates

   Also send the y coordinate inverted, because in OpenGL, Y positive is down, not up
*/
static const float window_texture_coord[][2] =
{ {-1, 1}, {-1, -1}, {1, -1},
  {-1, 1}, {1, 1}, {1, -1}};

//{ {0, 1 }, { 1, 1 }, { 1, 0}, {0, 1 }, {0, 0}, {1, 0}};


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

void GUIManager::initShaders(Window* w)
{
    int win_w, win_h;

    /* Get window size */
    w->GetSize(win_w, win_h);

    /* Compile the GUI shader */
    auto sFrag = new Shader{SHADERS_DIR "GUI.frag", SHADER_PIXEL};
    auto sVert = new Shader{SHADERS_DIR "GUI.vert", SHADER_VERTEX};
    if (!sFrag->Compile()) {
        throw shader_exception("GUI shader failed to compile", glGetError(),
			       sFrag->GetPath(), sFrag->GetType());
    }

    if (!sVert->Compile()) {
        throw shader_exception("GUI shader failed to compile", glGetError(),
			       sVert->GetPath(), sVert->GetType());
    }

    sGUI = new ShaderProgram{"gui", sVert, sFrag};
    if (!sGUI->Link()) {
        char shnum[6];
        sprintf(shnum, "%d", sGUI->GetID());
        throw shader_exception("GUI shader failed to link", glGetError(),
			       shnum, SHADER_PROGRAM);
    }


    attrPos = sGUI->GetAttributeLocation("position");
    attrTex = sGUI->GetAttributeLocation("in_uv");


    width = win_w;
    height = win_h;

    // Create the vertices
    glGenVertexArrays(1, &(this->vaoGUI));
    glBindVertexArray(this->vaoGUI);

    /* Create vertex information */
    glGenBuffers(1, &(this->vboPos));
    glBindBuffer(GL_ARRAY_BUFFER, this->vboPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18, window_pos_coord, GL_STATIC_DRAW);
    glVertexAttribPointer(this->attrPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(this->attrPos);

    glGenBuffers(1, &(this->vboTex));
    glBindBuffer(GL_ARRAY_BUFFER, this->vboTex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, window_texture_coord, GL_STATIC_DRAW);
    glVertexAttribPointer(this->attrTex, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(this->attrTex);

    glBindVertexArray(0);


    /* Create texture where we'll render the canvas */
    glGenTextures(1, &(texHandle));
    glBindTexture(GL_TEXTURE_2D, texHandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA,
		 GL_UNSIGNED_BYTE,
		 (void*)cairo_image_surface_get_data(this->canvas));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

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


/** Remove the control **/
void GUIManager::remove(GUIControl* c)
{    
    this->controls.erase(
	std::remove_if(this->controls.begin(), this->controls.end(), [&](GUIControl* control) {
		return control == c;
	    }));
    this->dirty = true;
}

/**
 * Try to handle the signal. Returns true if handled
 */
bool GUIManager::processSignal(GUISignal s)
{
    //Only natively process the redraw signal */
    if (s.signal == SignalType::Redraw) {
	// We redraw this by getting the width and height again

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
    bool isFocused = false;

    while (this->listener->PopEvent(ev)) {
	SignalType signalType = SignalType::Redraw;
	GUISignal gs;

	switch (ev.eventType) {
	case EVENT_MOUSEMOVE: signalType = SignalType::MouseHover; break;
	case EVENT_MOUSEEVENT:
	    signalType = SignalType::MouseClick;

	    gs.mouse.button = ev.event.mouseev.button;
	    gs.mouse.isPressed = ev.event.mouseev.status == KEY_KEYPRESS;
	    isFocused = true;

	    break;
	default: continue; // Ignore other events for now
	}

	gs.xPos = ev.mousex / this->width;
	gs.yPos = ev.mousey / this->height;
	gs.from = nullptr;
	gs.to = this;
	gs.signal = signalType;
	this->processSignal(gs);

    }

    if (isFocused) {
	this->listener->SetAccept();
    }

    this->listener->GetAcception(); // reset

    // ...and send them to the controls
    GUIControl::update();
    for (auto* control : controls) {
	control->update();
    }

}

/* Z-index comparator for the renderer */
struct ZIndexComparator {
    /* Returns true if a has a z-index bigger than b, i.e,
     * b would be drawn first, then a would be drawn over b
     */
    inline bool operator()(const GUIControl* a, const GUIControl* b) const {
	return (a->z_index > b->z_index);
    }
};

GUICanvas GUIManager::doRender(int absw, int absh) const
{
    (void)absw;
    (void)absh;

    // Clean bg
    cairo_set_source_rgba(ctxt, 0, 0, 0, 0);
    cairo_set_operator(ctxt, CAIRO_OPERATOR_SOURCE);
    cairo_paint(ctxt);


    std::priority_queue<GUIControl*, std::vector<GUIControl*>, ZIndexComparator> control_render_queue{
	ZIndexComparator(), this->controls};

    while (!control_render_queue.empty()) {

	auto control = control_render_queue.top();

	if (control->isDirty() || this->force_redraw)
	    control->render(this->width, this->height);

	unsigned absx = control->x * absw;
	unsigned absy = control->y * absh;

	auto ccanvas = control->getGUICanvas();
	
	cairo_set_operator(ctxt, CAIRO_OPERATOR_OVER);
	cairo_set_source_surface(ctxt, ccanvas, absx, absy);
	cairo_paint(ctxt);

	control_render_queue.pop();

    }

    return this->canvas;
}


/* Render this into a graphical framebuffer
 * that is 'screen' for you
 *
 * Puts the canvas into a gl texture and renders that texture to the screen using the
 * GUI shader
 */
void GUIManager::renderToScreen()
{

    // Make the GUI texture transparent
    glClearColor(0.0, 0.0, 0.0, 0.0);
    GLint depthf;
    glGetIntegerv(GL_DEPTH_FUNC, &depthf);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    sGUI->Use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texHandle);

    sGUI->SetUniform("texPanel", 0);
    sGUI->SetUniform("opacity", 1.0f);

    cairo_surface_flush(this->canvas);
    auto* canvas_data = cairo_image_surface_get_data(this->canvas);

    glBindVertexArray(this->vaoGUI);

    glEnableVertexAttribArray(attrPos);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glVertexAttribPointer(attrPos, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(attrTex);
    glBindBuffer(GL_ARRAY_BUFFER, vboTex);
    glVertexAttribPointer(attrTex, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindTexture(GL_TEXTURE_2D, this->texHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->width, this->height, 0, GL_BGRA,
		 GL_UNSIGNED_BYTE, canvas_data);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
	Log::GetLog()->Fatal("gui-renderer", "OpenGL error %#x", err);
    }

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDepthFunc(depthf);
    glDisable(GL_BLEND);

    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void GUIManager::render(int absw, int absh)
{
    auto c = this->doRender(this->width, this->height);

    this->canvas = c;

    this->force_redraw = false;
}

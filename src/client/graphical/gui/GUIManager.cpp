#include <client/graphical/gui/GUIManager.hpp>
#include <algorithm> // for remove_if

#include <client/graphical/shader_manager.hpp>
#include <client/input/input_service.hpp>
#include <common/Log.hpp>
#include <variant>

using namespace familyline::graphics;
using namespace familyline::graphics::gui;
using namespace familyline::input;

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
  {-1, 1}, {1, 1}, {1, -1} };


GUIManager::GUIManager(int width = 640, int height = 480)
{
    x = 0;
    y = 0;

    this->width = width;
    this->height = height;


    _listener = [&](HumanInputAction hia){

        SignalType signalType = SignalType::Redraw;
        GUISignal gs;
        bool ret = false;

        if (std::holds_alternative<MouseAction>(hia.type)) {
            signalType = SignalType::MouseHover;

            MouseAction ma = std::get<MouseAction>(hia.type);

            gs.xPos = ma.screenX / this->width;
            gs.yPos = ma.screenY / this->height;
            _isFocused = true;

			// TODO: only return false when not hovering any object
            ret = false;
        } else if (std::holds_alternative<ClickAction>(hia.type)){
            signalType = SignalType::MouseClick;

            ClickAction ca = std::get<ClickAction>(hia.type);

            gs.xPos = ca.screenX / this->width;
            gs.yPos = ca.screenY / this->height;
            gs.mouse.button = ca.buttonCode;
            gs.mouse.isPressed = ca.isPressed;
           
			ret = _isFocused;

        } else {
            return false;
        }

        gs.from = nullptr;
        gs.to = this;
        gs.signal = signalType;
        this->processSignal(gs);

        return ret;
    };

    input::InputService::getInputManager()->addListenerHandler(_listener);

    // Create the cairo context.
    // It wiil do this->width * 1

    this->setContext(1, 1);
}

void GUIManager::initShaders(Window* w)
{
    int win_w, win_h;

    /* Get window size */
    w->getSize(win_w, win_h);

    sGUI = new ShaderProgram("gui", {
            Shader("shaders/GUI.vert", ShaderType::Vertex),
            Shader("shaders/GUI.frag", ShaderType::Fragment)
        });

    sGUI->link();

    attrPos = 0; //sGUI->GetAttributeLocation("position");
    attrTex = 1; //sGUI->GetAttributeLocation("in_uv");

    width = win_w;
    height = win_h;
    this->setContext(1, 1);

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

    printf("%.2f %.2f", width, height);
    fflush(stdout);

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
    auto caddsig = GUISignal{ this, c, SignalType::ContainerAdd, 0, 0 };
    caddsig.absw = this->width;
    caddsig.absh = this->height;
    c->sendSignal(caddsig);

    caddsig = GUISignal{ this, c, SignalType::Redraw, 0, 0 };
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


/* Z-index comparator for the renderer */
struct ZIndexComparator {
    /* Returns true if a has a z-index bigger than b, i.e,
     * b would be drawn first, then a would be drawn over b
     *
     * Essentially, lower z-index will have more priority
     */
    inline bool operator()(const GUIControl* a, const GUIControl* b) const {
        return (a->z_index > b->z_index);
    }
};


/* Z-index reverse comparator for the renderer */
struct ZIndexReverseComparator {
    /* It's essentially the inverse of ZIndexComparator, because we need the higher z-index
     * to be the most prioritized instead of the lower ones
     */
    inline bool operator()(const GUIControl* a, const GUIControl* b) const {
        return (a->z_index < b->z_index);
    }
};


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
    }
    else {

        /* Pass the rest of  signals to its children */
        GUISignal ns = s;
        ns.from = this;

        /* Respect z-index when sending message to the controls */
        std::priority_queue<GUIControl*, std::vector<GUIControl*>, ZIndexReverseComparator> control_update_queue{
            ZIndexReverseComparator(), this->controls };


        while (!control_update_queue.empty()) {
            auto control = control_update_queue.top();

            if (s.xPos >= control->x && s.xPos <= (control->x + control->width) &&
                s.yPos >= control->y && s.yPos <= (control->y + control->height)) {
                ns.to = control;
                // Pass relative positions
                // TODO: Pass absolute positions somehow.
                // (maybe not, with SignalType::ContainerAdd this might not be needed)
                ns.xPos = s.xPos - control->x;
                ns.yPos = s.yPos - control->y;

                control->sendSignal(ns);

                if (s.signal == SignalType::MouseClick || s.signal == SignalType::MouseHover ||
                    s.signal == SignalType::KeyDown)
                    break;
            }

            control_update_queue.pop();
        }

    }

    return true;
}

// Process signals for all controls
void GUIManager::update()
{
    // Receive the input signals from the input listeners...
    _isFocused = false;


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


    std::priority_queue<GUIControl*, std::vector<GUIControl*>, ZIndexComparator> control_render_queue{
        ZIndexComparator(), this->controls };

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

#include <client/graphical/gui/../gfx_service.hpp>

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

    GFXService::getShaderManager()->use(*sGUI);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texHandle);

    sGUI->setUniform("texPanel", 0);
    sGUI->setUniform("opacity", 1.0f);

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
    (void)absw;
    (void)absh;

    auto c = this->doRender(this->width, this->height);

    this->canvas = c;

    this->force_redraw = false;
}

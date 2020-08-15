#include <GL/glew.h>

#include <client/graphical/exceptions.hpp>
#include <client/graphical/gfx_service.hpp>
#include <client/graphical/gui/gui_manager.hpp>

using namespace familyline::graphics::gui;
using namespace familyline::graphics;
using namespace familyline::input;

static int vv = 0;

/* The panel vertex square coordinates.
   It's a big rectangle, that fills the entire screen  */
static const float window_pos_coord[][3] = {
    {-1, 1, 1}, {-1, -1, 1}, {1, -1, 1}, {-1, 1, 1},
    {1, 1, 1},  {1, -1, 1}

    //  {-1, -1, 1}, { 1, -1, 1}, { 1, 1, 1},
    //  {-1, -1, 1}, {-1,  1, 1}, { 1, 1, 1}
};

/* Coordinates for every panel texture.
   Since they have the same vertex order, we don't need to declare multiple
   texture coordinates
   Also send the y coordinate inverted, because in OpenGL, Y positive is down, not up
*/
static const float window_texture_coord[][2] = {{-1, 1}, {-1, -1}, {1, -1},
                                                {-1, 1}, {1, 1},   {1, -1}};

/* Initialize shaders and window vertices.
 *
 * We render everything to a textured square. This function creates
 * the said square, plus the shaders that enable the rendering there
 */
void GUIManager::init(const Window& win)
{
    int win_w, win_h;

    /* Get window size */
    win.getSize(win_w, win_h);

    sGUI_ = new ShaderProgram(
        "gui", {Shader("shaders/GUI.vert", ShaderType::Vertex),
                Shader("shaders/GUI.frag", ShaderType::Fragment)});

    sGUI_->link();

    attrPos_ = 0;  // sGUI->GetAttributeLocation("position");
    attrTex_ = 1;  // sGUI->GetAttributeLocation("in_uv");

    width_  = (unsigned)win_w;
    height_ = (unsigned)win_h;

    // Create the vertices
    glGenVertexArrays(1, &(this->vaoGUI_));
    glBindVertexArray(this->vaoGUI_);

    /* Create vertex information */
    glGenBuffers(1, &(this->vboPos_));
    glBindBuffer(GL_ARRAY_BUFFER, this->vboPos_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18, window_pos_coord, GL_STATIC_DRAW);
    glVertexAttribPointer(this->attrPos_, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(this->attrPos_);

    glGenBuffers(1, &(this->vboTex_));
    glBindBuffer(GL_ARRAY_BUFFER, this->vboTex_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, window_texture_coord, GL_STATIC_DRAW);
    glVertexAttribPointer(this->attrTex_, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(this->attrTex_);

    glBindVertexArray(0);

    /* Create texture where we'll render the canvas */
    glGenTextures(1, &texHandle_);
    glBindTexture(GL_TEXTURE_2D, texHandle_);

    LOGDEBUG(LoggerService::getLogger(), "gui-manager", "gui size: %dx%d", width_, height_);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_BGRA, GL_UNSIGNED_BYTE,
        (void*)cairo_image_surface_get_data(this->canvas_));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        char e[128];
        snprintf(e, 127, "error %#x while setting texture for GUI content", err);
        throw graphical_exception(std::string(e));
    }
}

void GUIManager::add(int x, int y, Control* control)
{
    root_control_->getControlContainer()->add(x, y, std::unique_ptr<Control>(control));
}

void GUIManager::add(double x, double y, ControlPositioning cpos, Control* control)
{
    if (x > 1.1 || y > 1.1) this->add((int)x, (int)y, control);

    root_control_->getControlContainer()->add(x, y, cpos, std::unique_ptr<Control>(control));
}

void GUIManager::remove(Control* control)
{
    if (control) root_control_->getControlContainer()->remove(control->getID());
}

/**
 * Render the cairo canvas to the gui texture
 */
void GUIManager::renderToTexture()
{
    auto& log = LoggerService::getLogger();

    // Make the GUI texture transparent
    glClearColor(0.0, 0.0, 0.0, 0.0);
    GLint depthf;
    glGetIntegerv(GL_DEPTH_FUNC, &depthf);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    GFXService::getShaderManager()->use(*sGUI_);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texHandle_);

    sGUI_->setUniform("texPanel", 0);
    sGUI_->setUniform("opacity", 1.0f);

    cairo_surface_flush(this->canvas_);
    auto* canvas_data = cairo_image_surface_get_data(this->canvas_);

    glBindVertexArray(this->vaoGUI_);

    glEnableVertexAttribArray(attrPos_);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos_);
    glVertexAttribPointer(attrPos_, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(attrTex_);
    glBindBuffer(GL_ARRAY_BUFFER, vboTex_);
    glVertexAttribPointer(attrTex_, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindTexture(GL_TEXTURE_2D, this->texHandle_);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8, this->width_, this->height_, 0, GL_BGRA, GL_UNSIGNED_BYTE,
        canvas_data);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        log->write("gui-renderer", LogType::Error, "OpenGL error %#x", err);
    }

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDepthFunc(depthf);
    glDisable(GL_BLEND);

    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void GUIManager::update() { root_control_->update(context_, canvas_); }

void GUIManager::render(unsigned int x, unsigned int y) { this->renderToTexture(); }

GUIManager::~GUIManager()
{
    cairo_destroy(context_);
    cairo_surface_destroy(canvas_);

    // TODO: remove the input handler
}

/**
 * Get the control that is in the specified pixel coordinate
 */
std::optional<Control*> GUIManager::getControlAtPoint(int x, int y)
{
    return root_control_->getControlContainer()->getControlAtPoint(x, y);
}

bool GUIManager::checkIfEventHits(const HumanInputAction& hia)
{
    if (std::holds_alternative<GameExit>(hia.type)) {
        return false;
    }

    if (std::holds_alternative<ClickAction>(hia.type)) {
        auto ca = std::get<ClickAction>(hia.type);
        return this->getControlAtPoint(ca.screenX, ca.screenY).has_value();
    }

    if (std::holds_alternative<MouseAction>(hia.type)) {
        auto ma    = std::get<MouseAction>(hia.type);
        hitmousex_ = ma.screenX;
        hitmousey_ = ma.screenY;
        return this->getControlAtPoint(ma.screenX, ma.screenY).has_value();
    }

    if (std::holds_alternative<KeyAction>(hia.type)) {
        return this->getControlAtPoint(hitmousex_, hitmousey_).has_value();
    }

    if (std::holds_alternative<WheelAction>(hia.type)) {
        auto wa = std::get<WheelAction>(hia.type);
        return this->getControlAtPoint(wa.screenX, wa.screenY).has_value();
    }

    return false;
}

/**
 * Process received input events
 *
 */
void GUIManager::receiveEvent()
{
    while (!input_actions_.empty()) {
        auto& hia = input_actions_.front();
        root_control_->receiveEvent(hia, cb_queue_);

        input_actions_.pop();
    }
}

void GUIManager::runCallbacks()
{
    if (!cb_queue_.callbacks.empty()) {
        // Run one callback per call.
        auto cb = cb_queue_.callbacks.front();
        cb_queue_.callbacks.pop();

        // TODO: check if the owner exists.
        cb.fn(cb.owner);
    }
}

////////////////////////////////////////////////

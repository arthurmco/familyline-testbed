#include <GL/glew.h>
#include <client/graphical/gui/gui_manager.hpp>
#include <client/graphical/exceptions.hpp>
#include <client/graphical/gfx_service.hpp>

using namespace familyline::graphics::gui;
using namespace familyline::graphics;
using namespace familyline::input;

static int vv = 0;


/* The panel vertex square coordinates.
   It's a big rectangle, that fills the entire screen  */
static const float window_pos_coord[][3] =
{
    {-1, 1, 1}, {-1, -1, 1}, {1, -1, 1},
    {-1, 1, 1}, {1, 1, 1}, {1, -1, 1}

    //  {-1, -1, 1}, { 1, -1, 1}, { 1, 1, 1},
    //  {-1, -1, 1}, {-1,  1, 1}, { 1, 1, 1}
};

/* Coordinates for every panel texture.
   Since they have the same vertex order, we don't need to declare multiple
   texture coordinates
   Also send the y coordinate inverted, because in OpenGL, Y positive is down, not up
*/
static const float window_texture_coord[][2] =
{ {-1, 1}, {-1, -1}, {1, -1},
  {-1, 1}, {1, 1}, {1, -1} };


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

    sGUI_ = new ShaderProgram("gui", {
            Shader("shaders/GUI.vert", ShaderType::Vertex),
            Shader("shaders/GUI.frag", ShaderType::Fragment)
        });

    sGUI_->link();

    attrPos_ = 0; //sGUI->GetAttributeLocation("position");
    attrTex_ = 1; //sGUI->GetAttributeLocation("in_uv");

    width_ = (unsigned) win_w;
    height_ = (unsigned) win_h;

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

    LOGDEBUG(LoggerService::getLogger(), "gui-manager",
             "gui size: %dx%d", width_, height_);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE,
                 (void*)cairo_image_surface_get_data(this->canvas_));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
	
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        char e[128];
        snprintf(e, 127, "error %#x while setting texture for GUI content",
                 err);
        throw graphical_exception(std::string(e));
    }
}
        
void GUIManager::add(int x, int y, Control* control)
{
    root_control_->getControlContainer()->add(x, y, std::unique_ptr<Control>(control));    
}

void GUIManager::add(double x, double y, ControlPositioning cpos, Control* control)
{
    if (x > 1.1 || y > 1.1)
        this->add((int)x, (int)y, control);

    root_control_->getControlContainer()->add(x, y, cpos, std::unique_ptr<Control>(control));
}


void GUIManager::remove(Control* control)
{
    
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->width_, this->height_, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, canvas_data);

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


void GUIManager::update()
{
    root_control_->update(context_, canvas_);    
}

void GUIManager::render(unsigned int x, unsigned int y) {
    this->renderToTexture();    
}

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
        auto ma = std::get<MouseAction>(hia.type);
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
        root_control_->receiveEvent(hia);

        input_actions_.pop();        
    }
    /*
    switch (e.type) {
    case SDL_KEYDOWN:
        fprintf(stderr, "Key down: state=%s, repeat=%d, key=%08x, mod=%04x\n",
               e.key.state == SDL_PRESSED ? "pressed" : "released",
               e.key.repeat, e.key.keysym.sym, e.key.keysym.mod);
        break;

    case SDL_KEYUP:
        fprintf(stderr, "Key up: state=%s, repeat=%d, key=%08x, mod=%04x\n",
               e.key.state == SDL_PRESSED ? "pressed" : "released",
               e.key.repeat, e.key.keysym.sym, e.key.keysym.mod);
        break;

    case SDL_MOUSEBUTTONDOWN:
        fprintf(stderr, "Mouse button down: state=%s, clicks=%d, mouse=%d, button=%04x, x=%d, y=%d\n",
               e.button.state == SDL_PRESSED ? "pressed" : "released",
               e.button.clicks, e.button.which, e.button.button, e.button.x, e.button.y);
        break;

    case SDL_MOUSEBUTTONUP:
        fprintf(stderr, "Mouse button up: state=%s, clicks=%d, mouse=%d, button=%04x, x=%d, y=%d\n",
               e.button.state == SDL_PRESSED ? "pressed" : "released",
               e.button.clicks, e.button.which, e.button.button, e.button.x, e.button.y);
        break;

    case SDL_MOUSEMOTION:
        fprintf(stderr, "Mouse motion: mouse=%d, state=%04x, x=%d, y=%d, xdir=%d, ydir=%d\n",
               e.motion.which, e.motion.state, e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
        break;

    case SDL_MOUSEWHEEL:
        fprintf(stderr, "Mouse wheel: mouse=%d, x=%d, y=%d, direction=%s, \n",
               e.wheel.which, e.wheel.x, e.wheel.y,
               e.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? "normal" : "flipped");
        break;

    case SDL_TEXTEDITING:
        fprintf(stderr, "Text Editing: text='%s', start=%d, length=%d\n",
               e.edit.text, e.edit.start, e.edit.length);
        break;

    case SDL_TEXTINPUT:
        fprintf(stderr, "Text Input: text='%s'\n", e.text.text);
        break;

    case SDL_JOYAXISMOTION:
        fprintf(stderr, "Joy axis motion\n");
        break;

    case SDL_JOYBALLMOTION:
        fprintf(stderr, "Joy ball motion\n");
        break;

    case SDL_JOYHATMOTION:
        fprintf(stderr, "Joy hat motion\n");
        break;

    default:
        fprintf(stderr, "Unknown event: %08x\n", e.type);
        break;
    }

    root_control_->receiveEvent(e);
    */
}


////////////////////////////////////////////////

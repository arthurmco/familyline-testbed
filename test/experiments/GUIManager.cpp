#include "GUIManager.hpp"

using namespace familyline::graphics::gui;

static int vv = 0;

void GUIManager::update()
{
    lbl3->setText(std::to_string(vv));
    vv++;

    root_control_->update(context_, canvas_);

    uint32_t* fbdata = nullptr;
    uint32_t* cairodata = nullptr;
    int fbpitch = 0;

    // Lock texture access and c opy data from the cairo context to SDL
    SDL_LockTexture(framebuffer_, nullptr, (void**)&fbdata, &fbpitch);
    cairo_surface_flush(this->canvas_);

    cairodata = (uint32_t*)cairo_image_surface_get_data(this->canvas_);

    memcpy((void*)fbdata, (void*)cairodata, height_*fbpitch);

    SDL_UnlockTexture(framebuffer_);

    // render the updated texture
    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, framebuffer_, nullptr, nullptr);

}


GUIManager::~GUIManager()
{
    cairo_destroy(context_);
    cairo_surface_destroy(canvas_);

    SDL_DestroyTexture(framebuffer_);
}

/**
 * Receive an event and act on it
 *
 * (In the game, we will probably use input actions, not sdl events directly)
 */
void GUIManager::receiveEvent(const SDL_Event& e)
{
    switch (e.type) {
    case SDL_KEYDOWN:
        printf("Key down: state=%s, repeat=%d, key=%08x, mod=%04x\n",
               e.key.state == SDL_PRESSED ? "pressed" : "released",
               e.key.repeat, e.key.keysym.sym, e.key.keysym.mod);
        break;

    case SDL_KEYUP:
        printf("Key up: state=%s, repeat=%d, key=%08x, mod=%04x\n",
               e.key.state == SDL_PRESSED ? "pressed" : "released",
               e.key.repeat, e.key.keysym.sym, e.key.keysym.mod);
        break;

    case SDL_MOUSEBUTTONDOWN:
        printf("Mouse button down: state=%s, clicks=%d, mouse=%d, button=%04x, x=%d, y=%d\n",
               e.button.state == SDL_PRESSED ? "pressed" : "released",
               e.button.clicks, e.button.which, e.button.button, e.button.x, e.button.y);
        break;

    case SDL_MOUSEBUTTONUP:
        printf("Mouse button up: state=%s, clicks=%d, mouse=%d, button=%04x, x=%d, y=%d\n",
               e.button.state == SDL_PRESSED ? "pressed" : "released",
               e.button.clicks, e.button.which, e.button.button, e.button.x, e.button.y);
        break;

    case SDL_MOUSEMOTION:
        printf("Mouse motion: mouse=%d, state=%04x, x=%d, y=%d, xdir=%d, ydir=%d\n",
               e.motion.which, e.motion.state, e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
        break;

    case SDL_MOUSEWHEEL:
        printf("Mouse wheel: mouse=%d, x=%d, y=%d, direction=%s, \n",
               e.wheel.which, e.wheel.x, e.wheel.y,
               e.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? "normal" : "flipped");
        break;

    case SDL_TEXTEDITING:
        printf("Text Editing: text='%s', start=%d, length=%d\n",
               e.edit.text, e.edit.start, e.edit.length);
        break;

    case SDL_TEXTINPUT:
        printf("Text Input: text='%s'\n", e.text.text);
        break;

    case SDL_JOYAXISMOTION:
        puts("Joy axis motion");
        break;

    case SDL_JOYBALLMOTION:
        puts("Joy ball motion");
        break;

    case SDL_JOYHATMOTION:
        puts("Joy hat motion");
        break;
        
    default:
        printf("Unknown event: %08x\n", e.type);
        break;
    }

    root_control_->receiveEvent(e);
}


////////////////////////////////////////////////
#include <chrono>

Button::Button(unsigned width, unsigned height, std::string text)
    : width_(width), height_(height), label_(Label{1, 1, ""}) {

    // We set the text after we set the resize callback, so we can get the
    // text size correctly already when we build the button class
    label_.setResizeCallback([&](Control* c, size_t w, size_t h) {
        label_width_ = w;
        label_height_ = h;

        cairo_surface_destroy(l_canvas_);
        cairo_destroy(l_context_);
        
        l_canvas_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
        l_context_ = cairo_create(l_canvas_);

    });

    l_canvas_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    l_context_ = cairo_create(l_canvas_);
    label_.update(l_context_, l_canvas_);    
    label_.setText(text);
}


bool Button::update(cairo_t *context, cairo_surface_t *canvas)
{
    size_t label_x = width_/2 - label_width_/2; 
    size_t label_y = height_/2 - label_height_/2;       

    // draw the background
    if (clicked_ || std::chrono::steady_clock::now()-last_click_ < std::chrono::milliseconds(100)) {
        cairo_set_source_rgba(context, 0, 0, 0, 1.0);
    } else {
        cairo_set_source_rgba(context, 0, 0, 0, 0.5);        
    }
    cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
    cairo_paint(context);

    // draw a border
    cairo_set_line_width (context, 6);
    cairo_set_source_rgba (context, 0, 0, 0, 1);
    cairo_rectangle (context, 0, 0, width_, height_);
    cairo_stroke (context);

    label_.update(l_context_, l_canvas_);

    cairo_set_operator(context, CAIRO_OPERATOR_OVER);
    cairo_set_source_surface(context, l_canvas_, label_x, label_y);
    cairo_paint(context);

    return true;
}

std::tuple<int, int> Button::getNeededSize(cairo_t *parent_context) const
{
    return std::tie(width_, height_);
}

void Button::receiveEvent(const SDL_Event &e)
{
    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
        clicked_ = true;
        break;
        
    case SDL_MOUSEBUTTONUP:
        clicked_ = false;
        break;
    }

    if (clicked_) {
        click_active_ = true;
        click_fut_ = std::async(this->click_cb_, this);
    }
    
    // Separating the click action (the clicked_ variable) from the "is clicked" question
    // allows us to set the last click only when the user releases the click. This is a better
    // and more reliable way than activating it on the button down event, or when the clicked_
    // is false
    if (click_active_) {
        if (!clicked_) {
            last_click_ = std::chrono::steady_clock::now();
            click_active_ = false;
        }
    }
}


void Button::setText(std::string v)
{
    label_.setText(v);
}




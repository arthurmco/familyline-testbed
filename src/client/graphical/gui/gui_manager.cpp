#include <client/graphical/gui/gui_manager.hpp>

using namespace familyline::graphics::gui;

static int vv = 0;


void GUIManager::add(Control* control)
{
    
}

void GUIManager::remove(Control* control)
{
    
}


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
//    SDL_RenderClear(renderer_);
//    SDL_RenderCopy(renderer_, framebuffer_, nullptr, nullptr);

}

void GUIManager::render(unsigned int x, unsigned int y) {
    
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
}


////////////////////////////////////////////////

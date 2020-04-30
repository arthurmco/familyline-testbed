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

#include "GUIManager.hpp"

using namespace familyline::graphics::gui;

void GUIManager::update()
{
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


/////////////////////////////

bool Label::update(cairo_t* context, cairo_surface_t* canvas)
{
    cairo_set_source_rgba(context, 1, 1, 1, 1);
    cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
    cairo_paint(context);

    cairo_set_source_rgba(context, 0, 0, 0, 1);
    cairo_set_font_size(context, 24);
	cairo_select_font_face(context, "Arial",
                           CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

	cairo_text_extents_t te = {};
	cairo_text_extents(context, this->text_.c_str(), &te);
	cairo_move_to(context, 0, -te.y_bearing);
	cairo_show_text(context, this->text_.c_str());

    return true;
}

std::tuple<int, int> Label::getNeededSize(cairo_t* parent_context) const
{
    auto width = 1;
    auto height = 1;
    
    if (!parent_context) {
        return std::tie(width, height);
    }

	cairo_set_font_size(parent_context, 24);
	cairo_select_font_face(parent_context, "Arial",
		CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    cairo_text_extents_t te = {};
	cairo_text_extents(parent_context, this->text_.c_str(), &te);

    width = te.x_advance;
    height = te.height;

    return std::tie(width, height);
}

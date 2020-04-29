#include "Control.hpp"

using namespace familyline::graphics::gui;

/**
 * This is called by the control component when an object is added to it, so
 * you can create a context for it
 */
std::tuple<cairo_t*, cairo_surface_t*> Control::createChildContext(Control *c)
{
    auto [w, h] = c->getNeededSize(nullptr);
    cairo_surface_t* canvas = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
    cairo_t* context = cairo_create(canvas);

    if (w <= 1 || h <= 1) {
        auto [w, h] = c->getNeededSize(context);

        cairo_destroy(context);
        cairo_surface_destroy(canvas);

        canvas = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
        context = cairo_create(canvas);        
    }
    
    return std::tie(context, canvas);
}

////////////////////////////////////////////////////


void ContainerComponent::add(int x, int y, std::unique_ptr<Control> c)
{
    auto [context, canvas] = this->parent->createChildContext(c.get());

    this->children.emplace_back(x, y, context, canvas, std::move(c));
}

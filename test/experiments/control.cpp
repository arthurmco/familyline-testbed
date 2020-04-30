#include "Control.hpp"
#include <ctime>

using namespace familyline::graphics::gui;

Control::Control()
{
    id_ = clock();
    printf("id %08lx \n", id_);
}

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


/**
 * Resize the control
 */
void Control::resize(size_t w, size_t h)
{
    resize_cb_(this, w, h);
}

////////////////////////////////////////////////////


void ContainerComponent::add(int x, int y, std::unique_ptr<Control> c)
{
    c->setResizeCallback([&](Control* co, size_t w, size_t h) {
        auto co_it = std::find_if(this->children.begin(), this->children.end(),
                                  [&](ControlData& cd) {
                                      return cd.control->getID() == co->getID();
                                  });

        if (co_it != this->children.end()) {
            cairo_destroy(co_it->local_context);
            cairo_surface_destroy(co_it->control_canvas);

            co_it->control_canvas = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
            co_it->local_context = cairo_create(co_it->control_canvas);
        }
    });

    auto [context, canvas] = this->parent->createChildContext(c.get());
    this->children.emplace_back(x, y, context, canvas, std::move(c));
}

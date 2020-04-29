#include "root_control.hpp"

using namespace familyline::graphics::gui;


bool RootControl::update(cairo_t* context, cairo_surface_t* canvas)
{
    (void)context;
    
    // Clean bg
    cairo_set_source_rgba(context, 0, 0, 1, 1);
    cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
    cairo_paint(context);

    // Paint all children in the correct place
    for (auto& cdata : this->cc_->children) {
        cdata.control->update(cdata.local_context, cdata.control_canvas);

        cairo_set_operator(context, CAIRO_OPERATOR_OVER);
        cairo_set_source_surface(context, cdata.control_canvas, cdata.x, cdata.y);
        cairo_paint(context);
    }

    return true;
}

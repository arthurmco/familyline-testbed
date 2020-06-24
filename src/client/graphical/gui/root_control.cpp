#include <client/graphical/gui/root_control.hpp>

using namespace familyline::graphics::gui;


bool RootControl::update(cairo_t* context, cairo_surface_t* canvas)
{
    (void)context;

    // Clean bg
    cairo_set_source_rgba(context, 0.1, 0.4, 0.8, 1);
    cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
    cairo_paint(context);

    auto w = cairo_image_surface_get_width(canvas);
    auto h = cairo_image_surface_get_height(canvas);    
    
    // Paint all children in the correct place
    for (auto& cdata : this->cc_->children) {
        cdata.control->update(cdata.local_context, cdata.control_canvas);

        cairo_set_operator(context, CAIRO_OPERATOR_OVER);
        if (cdata.is_absolute) {
            cairo_set_source_surface(context, cdata.control_canvas, cdata.x,
                                   cdata.y);
        } else {
            auto absx = w * cdata.fx;
            auto absy = h * cdata.fy;
            printf("%.2f = %d * %.2f - ", absx, w, cdata.fx);
            printf("%.2f = %d * %.2f \n ", absy, h, cdata.fy);
            cairo_set_source_surface(context, cdata.control_canvas, absx, absy);
        }

        cairo_paint(context);
    }

    return true;
}

void RootControl::receiveEvent(const SDL_Event& e)
{
    for (auto& cdata : this->cc_->children) {
        auto [width, height] = cdata.control->getNeededSize(nullptr);

        switch (e.type) {
        case SDL_KEYDOWN:
//            printf("Key down: state=%s, repeat=%d, key=%08x, mod=%04x\n",
//                   e.key.state == SDL_PRESSED ? "pressed" : "released",
//                   e.key.repeat, e.key.keysym.sym, e.key.keysym.mod);
            break;

        case SDL_KEYUP:
//            printf("Key up: state=%s, repeat=%d, key=%08x, mod=%04x\n",
//                   e.key.state == SDL_PRESSED ? "pressed" : "released",
//                   e.key.repeat, e.key.keysym.sym, e.key.keysym.mod);
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
//            printf("Mouse button down: state=%s, clicks=%d, mouse=%d, button=%04x, x=%d, y=%d\n",
//                   e.button.state == SDL_PRESSED ? "pressed" : "released",
//                   e.button.clicks, e.button.which, e.button.button, e.button.x, e.button.y);
            if (e.button.x >= cdata.x && e.button.x <= cdata.x+width &&
                e.button.y >= cdata.y && e.button.y <= cdata.y+height) {

                SDL_Event ev = e;
                ev.button.x -= cdata.x;
                ev.button.y -= cdata.y;

                cdata.control->receiveEvent(ev);
                break;
            }

            break;

        case SDL_MOUSEMOTION:
//            printf("Mouse motion: mouse=%d, state=%04x, x=%d, y=%d, xdir=%d, ydir=%d\n",
//                   e.motion.which, e.motion.state, e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
            break;

        case SDL_MOUSEWHEEL:
//            printf("Mouse wheel: mouse=%d, x=%d, y=%d, direction=%s, \n",
//                   e.wheel.which, e.wheel.x, e.wheel.y,
//                   e.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? "normal" : "flipped");
            break;

        case SDL_TEXTEDITING:
//            printf("Text Editing: text='%s', start=%d, length=%d\n",
//                   e.edit.text, e.edit.start, e.edit.length);
            break;

        case SDL_TEXTINPUT:
//            printf("Text Input: text='%s'\n", e.text.text);
            break;

        case SDL_JOYAXISMOTION:
            break;

        case SDL_JOYBALLMOTION:
            break;

        case SDL_JOYHATMOTION:
            break;

        }

    }

}

#include <client/graphical/gui/gui_checkbox.hpp>
using namespace familyline::graphics::gui;

std::string GUICheckbox::describe() const
{
    char v[160] = {};
    sprintf(
        v, "GUICheckbox (id %08x, size %d x %d, pos %d,%d | status %s: '", id(), width_, height_,
        x_, y_, checked_ ? "checked" : "unchecked");
    auto ret = std::string{v};

    if (parent_) ret += "has a parent ";

    if (onFocus_) ret += " | focus";

    ret += ")";

    return ret;
}

void GUICheckbox::onResize(int nwidth, int nheight, int nx, int ny)
{
    width_  = nwidth;
    height_ = nheight;
    x_      = nx;
    y_      = ny;
}

void GUICheckbox::receiveInput(const familyline::input::HumanInputAction &e)
{
    using namespace familyline::input;

    if (auto *kev = std::get_if<KeyAction>(&e.type); kev) {
        if (kev->keycode == SDLK_RETURN && kev->isPressed) {
            checked_ = !checked_;
            dirty_   = true;
        }
    } else if (auto *mev = std::get_if<ClickAction>(&e.type); mev) {
        if (!mev->isPressed) {
            checked_ = !checked_;
            dirty_   = true;
        }
    }
}

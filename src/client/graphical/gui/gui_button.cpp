#include <client/graphical/gui/gui_button.hpp>
#include <client/input/input_actions.hpp>

using namespace familyline::graphics::gui;


std::string GUIButton::describe() const {
  char v[128] = {};
  sprintf(v, "GUIButton (id %08x, size %d x %d, pos %d,%d | text: '", id(),
          width_, height_, x_, y_);
  auto ret = std::string{v};
  ret += label_.text();
  ret += "' ";

  if (parent_)
    ret += "has a parent ";

  if (onFocus_)
    ret += " | focus";

  ret += ")";

  return ret;
}

void GUIButton::onResize(int nwidth, int nheight, int nx, int ny) {
  width_ = nwidth;
  height_ = nheight;
  x_ = nx;
  y_ = ny;
}

void GUIButton::receiveInput(const familyline::input::HumanInputAction &e) {
    using namespace familyline::input;
    
    if (auto *kev = std::get_if<KeyAction>(&e.type); kev) {
        if (kev->keycode == SDLK_RETURN && kev->isPressed) {
            onClick_(*this);
        }
    } else if (auto *mev = std::get_if<ClickAction>(&e.type); mev) {
        if (mev->isPressed) {
            hover_ = true;
            onClick_(*this);            
        }
    } else if (auto *mev = std::get_if<MouseAction>(&e.type); mev) {
        hover_ = true;
    }

}

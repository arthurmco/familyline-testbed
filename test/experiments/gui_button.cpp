#include "gui_button.hpp"


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

void GUIButton::receiveInput(const GUIEvent &e) {
  if (auto *kev = std::get_if<KeyEvent>(&e); kev) {
    if (kev->key == '\r' && kev->isPressing) {
      onClick_(*this);
    }
  }
}

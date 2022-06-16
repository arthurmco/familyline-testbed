#include <client/graphical/gui/gui_control.hpp>

using namespace familyline::graphics::gui;


/**
 * Gets a no-op callback register, just so that we do not call a null pointer
 * when we do not have it
 */
FGUICallbackRegister familyline::graphics::gui::getDefaultCallbackRegister() {
    return [](auto fn, auto& control) {};
}


std::string GUIControl::describe() const {
  char v[128] = {};
  sprintf(v, "GUIControl (id %08x, size %d x %d)", id_, width_, height_);
  return std::string{v};
}

void GUIControl::onResize(int nwidth, int nheight, int nx, int ny)
{
  dirty_ = (width_ != nwidth || height_ != nheight || x_ != nx || y_ != ny);
  
  width_ = nwidth;
  height_ = nheight;
  x_ = nx;
  y_ = ny;
}


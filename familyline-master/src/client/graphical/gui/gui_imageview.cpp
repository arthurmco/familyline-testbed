#include <client/graphical/gui/gui_imageview.hpp>
using namespace familyline::graphics::gui;

std::string GUIImageView::describe() const {
  char v[128] = {};
  sprintf(v, "GUIImageView (id %08x, size %d x %d \n  (original %d x %d), pos %d,%d '", id(),
          width_, height_, image_->w, image_->h, x_, y_);
  auto ret = std::string{v};

  if (parent_)
    ret += "has a parent ";

  if (onFocus_)
    ret += " | focus";

  ret += ")";

  return ret;
}



/// Called when this control is resized or repositioned
void GUIImageView::onResize(int nwidth, int nheight, int nx, int ny)
{
    width_ = nwidth;
    height_ = nheight;
    x_ = nx;
    y_ = ny;

    draw_width_ = width_;
    draw_height_ = height_;

    dirty_ = true;
}

void GUIImageView::autoresize()
{
    width_ = image_->w;
    height_ = image_->h;
    dirty_ = true;
}

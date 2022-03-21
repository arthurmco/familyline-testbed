#include <fmt/core.h>

#include <client/graphical/gui/gui_label.hpp>

using namespace familyline::graphics::gui;

std::string GUILabel::describe() const
{
    return fmt::format(
        "GUILabel (id {:08x}, size {}x{}, pos {},{} | text: '{}', {} | {})", id(), width_, height_,
        x_, y_, text(), parent_ ? "has a parent" : "", onFocus_ ? "| focus" : "");
}

void GUILabel::calculateNeededSize()
{
    textwidth_  = text_.size() * appearance_.fontsize;
    textheight_ = appearance_.fontsize * 1.25;
    dirty_      = true;
}

void GUILabel::autoresize() { this->onResize(textwidth_, textheight_, x_, y_); }

void GUILabel::onResize(int nwidth, int nheight, int nx, int ny)
{
    width_  = nwidth;
    height_ = nheight;
    x_      = nx;
    y_      = ny;
}

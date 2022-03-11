#include <client/graphical/gui/gui_label.hpp>
using namespace familyline::graphics::gui;

std::string GUILabel::describe() const
{
    char v[128] = {};
    sprintf(
        v, "GUILabel (id %08x, size %d x %d, pos %d,%d | text: %s'", id(), width_, height_, x_, y_,
        text_.c_str());
    auto ret = std::string{v};

    if (parent_) ret += "has a parent ";

    if (onFocus_) ret += " | focus";

    ret += ")";

    return ret;
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

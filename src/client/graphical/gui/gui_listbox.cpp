#include <client/graphical/gui/gui_listbox.hpp>
#include <client/input/input_actions.hpp>
#include <range/v3/all.hpp>

using namespace familyline::graphics::gui;

GUIListbox::GUIListbox(std::vector<std::string> items, GUIControlRenderInfo i) : GUIControl(i)
{
    items_ = items | ranges::views::transform([](std::string v) { return std::make_pair(v, v); }) |
             ranges::to<std::vector>();
}

std::string GUIListbox::describe() const
{
    char v[128] = {};
    sprintf(
        v, "GUIListbox (id %08x, size %d x %d, pos %d,%d | itemcount: %zu ", id(), width_, height_,
        x_, y_, items_.size());
    auto ret = std::string{v};

    if (parent_) ret += "has a parent ";

    if (onFocus_) ret += " | focus";

    ret += ")";

    return ret;
}

void GUIListbox::iterItems(std::function<void(size_t, std::string)> fn) const
{
    auto index = 0;
    for (auto it : items_) {
        fn(index, it.second);
        index++;
    }
}

void GUIListbox::autoresize() {}

void GUIListbox::onResize(int nwidth, int nheight, int nx, int ny)
{
    width_  = nwidth;
    height_ = nheight;
    x_      = nx;
    y_      = ny;
}

void GUIListbox::receiveInput(const familyline::input::HumanInputAction &e)
{
    using namespace familyline::input;

    if (auto *mev = std::get_if<ClickAction>(&e.type); mev) {
        if (mev->isPressed) {
            int yoff = mev->screenY - y();
            dirty_   = true;

            if (yoff < 0) {
                selectedIndex_ = std::nullopt;
                return;
            }

            size_t idx = yoff / itemHeight_;
            if (idx >= items_.size()) {
                selectedIndex_ = std::nullopt;
                return;
            }

            auto newindex = std::make_optional(idx);
            if (selectedIndex_ != newindex) {
                onSelItemChange_((GUIControl &)*this, idx, items_[idx].first);
            }

            selectedIndex_ = newindex;
        }
    }
}

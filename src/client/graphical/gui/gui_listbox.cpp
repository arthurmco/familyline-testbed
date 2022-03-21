#include <fmt/core.h>

#include <algorithm>
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
    return fmt::format(
        "GUIListbox (id {:08x}, size {}x{}, pos {},{} | itemcount: {}, {} | {})", id(), width_,
        height_, x_, y_, items_.size(), parent_ ? "has a parent" : "", onFocus_ ? "| focus" : "");
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

void GUIListbox::add(std::string tag, std::string item)
{
    if (std::find_if(items_.begin(), items_.end(), [&](auto it) { return it.first == tag; }) !=
        items_.end()) {
        set(tag, item);
    } else {
        items_.emplace_back(std::make_pair(tag, item));
    }
}

void GUIListbox::set(std::string tag, std::string newitem)
{
    auto it =
        std::find_if(items_.begin(), items_.end(), [&](auto item) { return item.first == tag; });

    if (it != items_.end()) {
        it->second = newitem;
    }
}

void GUIListbox::remove(std::string tag)
{
    auto it =
        std::remove_if(items_.begin(), items_.end(), [&](auto item) { return item.first == tag; });

    items_.erase(it, items_.end());
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

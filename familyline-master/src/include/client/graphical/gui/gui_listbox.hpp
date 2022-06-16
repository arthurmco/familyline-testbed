#pragma once

#include <client/graphical/gui/gui_control.hpp>
#include <string>
#include <vector>

namespace familyline::graphics::gui
{
using FGUISelectedListChange =
    std::function<void(GUIControl &, size_t /* index */, std::string /* tag */)>;

/**
 * GUIListbox
 *
 * A listbox
 * Stores a list of values. The values are formattable.
 */
class GUIListbox : public GUIControl
{
public:
    GUIListbox(std::vector<std::string> items, GUIControlRenderInfo i = {});

    GUIListbox(std::vector<std::pair<std::string, std::string>> items, GUIControlRenderInfo i = {})
        : GUIControl(i), items_(items){};

    /**
     * A textual way of describing the control
     * If we were in Python, this would be its `__repr__` method
     *
     * Used *only* for debugging purposes.
     */
    virtual std::string describe() const;

    /// Called when this control is resized or repositioned
    virtual void onResize(int nwidth, int nheight, int nx, int ny);

    virtual void autoresize();

    /// Called when the parent need to update
    virtual void update() { dirty_ = false; };

    int getItemHeight() const { return itemHeight_; }

    void iterItems(std::function<void(size_t, std::string)> fn) const;
    std::optional<size_t> const getSelectedIndex() { return selectedIndex_; };

    virtual void receiveInput(const familyline::input::HumanInputAction &e);

    void onSelectedItemChange(FGUISelectedListChange fn) { onSelItemChange_ = fn; };

    void add(std::string item) { add(item, item); }
    void add(std::string tag, std::string item);

    void set(std::string tag, std::string newitem);

    void remove(std::string tag);

private:
    int itemHeight_ = 20;
    std::optional<size_t> selectedIndex_;
    FGUISelectedListChange onSelItemChange_ = [](GUIControl &, size_t, std::string) {};

    std::vector<std::pair<std::string, std::string>> items_;
};

}  // namespace familyline::graphics::gui

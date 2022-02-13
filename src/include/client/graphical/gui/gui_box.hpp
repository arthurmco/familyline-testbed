#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <cassert>
#include <client/graphical/gui/gui_control.hpp>
#include <client/graphical/gui/gui_layout.hpp>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

namespace familyline::graphics::gui
{
/**
 * GUIBox
 *
 * Can house widgets without being a window
 * It can be initialized with a layout, and the children of this
 * box will act like this is a window anyway
 *
 * Obligatory for more complex layouts.
 *
 * TODO: finish the GUIBox
 */
class GUIBox : public GUIControl
{
    friend class GUIWindow;

public:
    GUIBox(BaseLayout &layout, GUIControlRenderInfo render_info = {})
        : GUIControl(render_info), layout_(layout)
    {
        this->initLayout(layout);
    }

    auto begin() { return controls_.begin(); }
    auto end() { return controls_.end(); }
    size_t control_count() { return controls_.size(); }

    GUIControl &add(GUIControl *);
    void remove(GUIControl &);

    /**
     * Get a certain control by its ID
     */
    GUIControl *get(int id) const;

    /// A textual way of describing the control
    /// If we were in Python, this would be its `__repr__` method
    ///
    /// Used *only* for debugging purposes.
    virtual std::string describe() const;

    /// Called when this control is resized or repositioned
    virtual void onResize(int nwidth, int nheight, int nx, int ny);

    virtual void autoresize();

    /// Called when the parent need to update
    virtual void update();

    virtual void receiveInput(const familyline::input::HumanInputAction &e);

    /**
     * Forward the tab index event (aka the act of changing
     * control focus when you press TAB) to the next
     * element
     *
     * Returns true while we have elements to tab, false when we do not have any
     */
    bool forwardTabIndexEvent();

    virtual void onFocusExit()
    {
        onFocus_ = false;

        if (tab_index_ < 0) return;

        auto tab_control = controls_.begin();
        std::advance(tab_control, tab_index_);
        (*tab_control)->onFocusExit();

        if (last_focus_control_id >= 0 && tab_index_ != focused_index_) {
            tab_control = controls_.begin();
            std::advance(tab_control, last_focus_control_id);
            (*tab_control)->onFocusExit();
        }

        tab_index_ = -1;
    }

    virtual void setEventCallbackRegisterFunction(FGUICallbackRegister r);

private:
    ssize_t previous_tab_index_ = -1;
    ssize_t tab_index_          = -1;
    bool is_tab                 = false;

    int last_focus_control_id = -1;

    /**
     * Set the minimum and maximum sizes of this box according to
     * our children
     */
    void resetBoundarySizes();

    // The index of the currently focused control.
    // TODO: reset this if the removed control is focused.
    ssize_t focused_index_ = -1;

    bool inner_box_ = false;

    BaseLayout &layout_;
    std::vector<GUIControl *> controls_;

    void initLayout(BaseLayout &layout);
};
}  // namespace familyline::graphics::gui

#pragma once

#include <client/graphical/gui/gui_control.hpp>
#include <string>

namespace familyline::graphics::gui
{
/**
 * GUICheckbox
 *
 * A checkbox
 * Stores a boolean state
 */
class GUICheckbox : public GUIControl
{
public:
    GUICheckbox(bool checked, GUIControlRenderInfo i = {}) : GUIControl(i), checked_(checked)
    {
        this->appearance_.maxHeight = 40;
    };

    /**
     * A textual way of describing the control
     * If we were in Python, this would be its `__repr__` method
     *
     * Used *only* for debugging purposes.
     */
    virtual std::string describe() const;

    /// Called when this control is resized or repositioned
    virtual void onResize(int nwidth, int nheight, int nx, int ny);

    /// Called when the parent need to update
    virtual void update() { dirty_ = false; };

    virtual void receiveInput(const familyline::input::HumanInputAction &e);

    bool checked() const { return checked_; }

private:
    bool checked_ = false;
};

}  // namespace familyline::graphics::gui

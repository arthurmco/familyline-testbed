#pragma once

#include <client/graphical/gui/gui_control.hpp>
#include <client/graphical/gui/gui_label.hpp>

namespace familyline::graphics::gui
{
/**
 * GUIButton
 *
 * Stores a button
 * A button is, of course, a control that you can click
 */

class GUIButton : public GUIControl
{
public:
    GUIButton(std::string text, FGUIEventCallback onClick, GUIControlRenderInfo i = {})
        : GUIControl(i), label_(GUILabel{text}), onClick_(onClick){};

    /// A textual way of describing the control
    /// If we were in Python, this would be its `__repr__` method
    ///
    /// Used *only* for debugging purposes.
    virtual std::string describe() const;

    /// Called when this control is resized or repositioned
    virtual void onResize(int nwidth, int nheight, int nx, int ny);

    //  virtual void autoresize();

    /// Called when the parent need to update
    virtual void update()
    {
        label_.update();
        dirty_ = false;
    };

    std::string text() const { return label_.text(); }
    void setText(std::string v)
    {
        label_.setText(v);
        dirty_ = true;
    }

    // TODO: use that badge thing from SerenityOS?
    GUILabel &getInnerLabel() { return label_; }

    // Focus enter and exit callbacks
    virtual void onFocusEnter()
    {
        onFocus_ = true;
        label_.onFocusEnter();
    }
    virtual void onFocusExit()
    {
        onFocus_ = false;
        label_.onFocusExit();
        hover_ = false;
    }

    bool isHover() const { return hover_; }
    
    virtual void receiveInput(const familyline::input::HumanInputAction &e);

private:
    GUILabel label_;

    bool hover_ = false;
    
    FGUIEventCallback onClick_;
};
}  // namespace familyline::graphics::gui

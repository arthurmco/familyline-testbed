#pragma once

#include <string>

#include "gui_control.hpp"

/**
 * GUILabel
 *
 * A text label
 * Simply stores a text
 */
class GUILabel : public GUIControl
{
public:
    GUILabel(std::string text, GUIControlRenderInfo i = {}) : GUIControl(i), text_(text)
    {
        calculateNeededSize();
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

    virtual void autoresize();

    /// Called when the parent need to update
    virtual void update() { dirty_ = false; };

    std::string text() const { return text_; }
    void setText(std::string v)
    {
        text_  = v;
        dirty_ = true;
    }

private:
    std::string text_;

    // Calculate how much size we need for the text width and height.
    int textwidth_;
    int textheight_;

    void calculateNeededSize();
};

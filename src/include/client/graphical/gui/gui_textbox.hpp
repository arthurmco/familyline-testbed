#pragma once

#include <client/graphical/gui/gui_control.hpp>
#include <optional>
#include <string>
#include <tuple>

namespace familyline::graphics::gui
{
/**
 * GUITextbox
 *
 * Allow the user to input text.
 *
 * It is not a very hard control to get, but due to things like IME and
 * font metrics and whatnot, it is hard to get it right
 *
 * Ideally, this textbox will implement things as correctly as possible,
 * i.e, with IME support, proper metrics for fonts outside of the ascii
 * standard, etc.
 */
class GUITextbox : public GUIControl
{
public:
    GUITextbox(std::string text, GUIControlRenderInfo i = {}) : GUIControl(i), text_(toU32(text)) {}

    /// A textual way of describing the control
    /// If we were in Python, this would be its `__repr__` method
    ///
    /// Used *only* for debugging purposes.
    virtual std::string describe() const;

    std::string text() const;

    /**
     * Get the text data in selection blocks.
     *
     * Returns a tuple, where:
     * - the first element is the text before the selection
     * - the second element is the selected text, or "" if no text is selected.
     * - the third element is the text after the selection
     *
     * The 'block' parameter allows rendering a block cursor, where the cursor
     * 'selects' the current character.
     */
    std::tuple<std::string, std::string, std::string> getTextAsSelection(bool block) const;

    /**
     * Get the selection starting and ending points
     */
    std::pair<size_t, size_t> getSelection() const
    {
        return std::make_pair(select_start_, select_end_);
    }

    // Focus enter and exit callbacks
    virtual void onFocusEnter()
    {
        GUIControl::onFocusEnter();
        render_info.setTextInputMode(true);
    }
    virtual void onFocusExit()
    {
        GUIControl::onFocusExit();
        render_info.setTextInputMode(false);
    }

    virtual void receiveInput(const familyline::input::HumanInputAction& e);

private:
    /// A string of text
    ///
    /// This is not a common string, but a string where a char is 4 bytes.
    /// It seems wasteful, but since we reached 2 bytes just now on the utf
    /// codepoint list, it seems easier to have one codepoint per 'char'
    std::u32string text_;

    /// Convert a normal string to a utf-32 string
    std::u32string toU32(std::string) const;

    /// Convert a utf-32 string to a normal string
    std::string toNormalString(std::u32string) const;

    size_t select_start_ = 0;
    size_t select_end_   = 0;

    /**
     * Given a X and Y position relative to the start of the control,
     * give back the character position closest to that point
     */
    size_t getCharFromPosition(size_t x, size_t y);
};

}  // namespace familyline::graphics::gui

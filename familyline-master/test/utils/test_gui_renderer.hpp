#pragma once

#include <client/graphical/gui/gui_renderer.hpp>

class TestControlPaintData : public familyline::graphics::gui::ControlPaintData
{
public:
    TestControlPaintData(
        familyline::graphics::gui::GUIControl &control, int width, int height, int x, int y,
        std::vector<std::unique_ptr<TestControlPaintData>> children = {})
        : control(control), width(width), height(height), x(x), y(y), children_(std::move(children))
    {
    }

    virtual std::string format() const { return "test"; }
    virtual uint8_t *data() const { return (uint8_t *)&control; }

    familyline::graphics::gui::GUIControl &control;
    int width, height, x, y;

    std::vector<std::unique_ptr<TestControlPaintData>> &children() { return children_; }

private:
    std::vector<std::unique_ptr<TestControlPaintData>> children_;
};

/**
 * A mock GUI renderer, just so we can test the GUI routines
 */
class TestGUIRenderer : public familyline::graphics::gui::GUIRenderer
{
public:
    /// Update the rendered content
    virtual void update(const std::vector<familyline::graphics::gui::ControlPaintData *> &data);

    /// Render the data
    virtual void render();

    virtual void onResize(int width, int height)
    {
        screenWidth_  = width;
        screenHeight_ = height;
    }

    virtual std::optional<familyline::graphics::gui::GUIGlyphSize> getCodepointSize(
        char32_t codepoint, std::string_view fontName, size_t fontSize,
        familyline::graphics::gui::FontWeight weight);

    virtual std::unique_ptr<familyline::graphics::gui::GUIControlPainter> createPainter();

    int getScreenWidth() const { return screenWidth_; }
    int getScreenHeight() const { return screenHeight_; }

    /**
     * Make the graphical framework above us set some sort of text
     * input mode (SDL, for example, has SDL_StartTextInput)
     *
     * This function will take care of all IME related things
     * (for example, by combining ´ and a to form á, or by converting
     *  hiragana into kanji), sending all the steps until the final
     * character
     *
     * We make the renderer provide this information, but it might not
     * be the best place (we will probably need to rename the renderer
     * class)
     */
    virtual void setTextInputMode(bool v) {}

    /**
     * Print to a "virtual" debug pane
     *
     * This call is guaranteed to be called right before the render() method.
     */
    virtual void debugWrite(std::string) {}

    virtual ~TestGUIRenderer() {}

    TestControlPaintData *query(int id);

private:
    std::vector<familyline::graphics::gui::ControlPaintData *> data_;

    TestControlPaintData *queryInto(int id, TestControlPaintData *parent);

    int screenWidth_  = 1;
    int screenHeight_ = 1;
};

class TestControlPainter : public familyline::graphics::gui::GUIControlPainter
{
public:
    TestControlPainter(TestGUIRenderer &cr) : cr_(cr) {}

    virtual std::unique_ptr<familyline::graphics::gui::ControlPaintData> drawWindow(
        familyline::graphics::gui::GUIWindow &w);
    virtual std::unique_ptr<familyline::graphics::gui::ControlPaintData> drawControl(
        familyline::graphics::gui::GUIControl &c);

private:
    TestGUIRenderer &cr_;
};

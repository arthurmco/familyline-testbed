#pragma once

#include <client/graphical/gui/gui_renderer.hpp>

namespace familyline::graphics::gui {
    
class GLControlPaintData : public ControlPaintData
{
public:
    GLControlPaintData(
        GUIControl &control, int width, int height, int x, int y,
        std::vector<std::unique_ptr<GLControlPaintData>> children = {})
        : control(control), width(width), height(height), x(x), y(y), children_(std::move(children))
    {
    }

    virtual std::string format() const { return "test"; }
    virtual uint8_t *data() const { return (uint8_t *)&control; }

    GUIControl &control;
    int width, height, x, y;

    std::vector<std::unique_ptr<GLControlPaintData>> &children() { return children_; }

private:
    std::vector<std::unique_ptr<GLControlPaintData>> children_;
};

/**
 * A mock GUI renderer, just so we can test the GUI routines
 */
class GLGUIRenderer : public GUIRenderer
{
public:
    /// Update the rendered content
    virtual void update(const std::vector<ControlPaintData *> &data);

    /// Render the data
    virtual void render();

    virtual void onResize(int width, int height)
    {
        screenWidth_  = width;
        screenHeight_ = height;
    }

    virtual std::optional<GUIGlyphSize> getCodepointSize(
        char32_t codepoint, std::string_view fontName, size_t fontSize,
        FontWeight weight);

    virtual std::unique_ptr<GUIControlPainter> createPainter();

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

    virtual ~GLGUIRenderer() {}

    GLControlPaintData *query(int id);

private:
    std::vector<ControlPaintData *> data_;

    GLControlPaintData *queryInto(int id, GLControlPaintData *parent);

    int screenWidth_  = 1;
    int screenHeight_ = 1;
};

class GLControlPainter : public GUIControlPainter
{
public:
    GLControlPainter(GLGUIRenderer &cr) : cr_(cr) {}

    virtual std::unique_ptr<ControlPaintData> drawWindow(
        GUIWindow &w);
    virtual std::unique_ptr<ControlPaintData> drawControl(
        GUIControl &c);

private:
    GLGUIRenderer &cr_;
};

}

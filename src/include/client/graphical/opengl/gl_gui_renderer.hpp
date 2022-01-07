#pragma once

#include <cairo/cairo.h>

#include <client/graphical/gui/gui_renderer.hpp>
#include <client/graphical/opengl/gl_headers.hpp>
#include <client/graphical/shader.hpp>
#include "client/input/input_service.hpp"

#ifdef RENDERER_OPENGL

namespace familyline::graphics::gui
{
class GLControlPaintData : public ControlPaintData
{
public:
    GLControlPaintData(GUIControl &control, int x, int y, unsigned width, unsigned height)
        : control(control), canvas_(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height))
    {
        context = cairo_create(canvas_);
    }

    virtual std::string format() const { return "glrenderer"; }
    virtual uint8_t *data() const { return (uint8_t *)canvas_; }

    cairo_t *context;

    int x, y;

    GUIControl &control;

    virtual ~GLControlPaintData()
    {
        cairo_surface_destroy(canvas_);
        cairo_destroy(context);
    }

private:
    cairo_surface_t *canvas_;
};

/**
 * A mock GUI renderer, just so we can test the GUI routines
 */
class GLGUIRenderer : public GUIRenderer
{
public:
    GLGUIRenderer();

    /// Update the rendered content
    virtual void update(const std::vector<ControlPaintData *> &data);

    /// Render the data
    virtual void render();

    virtual void onResize(int width, int height)
    {
        screenWidth_  = width;
        screenHeight_ = height;
        tex_gui_      = this->resizeTexture(width, height);
    }

    virtual std::optional<GUIGlyphSize> getCodepointSize(
        char32_t codepoint, std::string_view fontName, size_t fontSize, FontWeight weight);

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
    virtual void setTextInputMode(bool v) {
        if (v)
            input::InputService::getInputManager()->enableTextEvents();
        else
            input::InputService::getInputManager()->disableTextEvents();

    }

    virtual ~GLGUIRenderer() {}

private:
    std::vector<ControlPaintData *> data_;

    cairo_surface_t *canvas_;
    cairo_t *context_;

    ShaderProgram *sGUI_ = nullptr;
    GLuint vao_gui_;
    GLuint tex_gui_;

    /**
     * Initialize the GUI shaders
     */
    void initShaders();

    /**
     * Initialize a texture, where the GUI contents will be drawn
     */
    GLuint initTexture(int width, int height);

    /**
     * Resize the GUI texture
     */
    GLuint resizeTexture(int width, int height);

    int screenWidth_  = 320;
    int screenHeight_ = 240;
};

class GLControlPainter : public GUIControlPainter
{
public:
    GLControlPainter(GLGUIRenderer &cr) : cr_(cr) {}

    virtual std::unique_ptr<ControlPaintData> drawWindow(GUIWindow &w);
    virtual std::unique_ptr<ControlPaintData> drawControl(GUIControl &c);

private:
    GLGUIRenderer &cr_;
};

}  // namespace familyline::graphics::gui

#endif

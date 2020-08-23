#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <cairo/cairo.h>
#include <pango/pangocairo.h>

#include <client/graphical/gui/gui_button.hpp>
#include <client/graphical/gui/gui_imageview.hpp>
#include <client/graphical/gui/gui_label.hpp>
#include <client/graphical/gui/root_control.hpp>
#include <client/graphical/shader.hpp>
#include <client/graphical/window.hpp>
#include <client/input/input_manager.hpp>
#include <common/logger.hpp>
#include <memory>
#include <queue>
#include <span>
#include <string>
#include <vector>

namespace familyline::graphics::gui
{
/// TODO: PLEASE find a way to run the GUI update function always in tribalia.cpp, or
///       familyline.cpp if renamed. It will make things easier.

/**
 * Manages the graphical interface state and rendering
 */
class GUIManager
{
private:
    familyline::graphics::Window& win_;

    familyline::graphics::ShaderProgram* sGUI_;
    GLuint vaoGUI_, attrPos_, vboPos_, attrTex_, vboTex_, texHandle_;

    std::array<unsigned int, 32 * 32> ibuf;

    unsigned width_, height_;

    // Cairo things, to actually do the rendering work
    // Since Cairo can use hardware acceleration, we do not need to worry much about speed.
    cairo_t* context_;
    cairo_surface_t* canvas_;

    std::unique_ptr<RootControl> root_control_;

    Label* lbl3;
    Label* lbl4;

    // Cached mouse positions, to help check events that do not send the mouse position along them,
    // like the KeyEvent
    int hitmousex_ = 1, hitmousey_ = 1;
    std::queue<familyline::input::HumanInputAction> input_actions_;

    CallbackQueue cb_queue_;

    /// TODO: add a way to lock event receiving to the GUI. Probably the text edit control
    /// will need, to ensure you can type on it when you click and continue to be able to,
    /// even if you move the mouse out of it.

    /**
     * Initialize shaders and window vertices.
     *
     * We render everything to a textured square. This function creates
     * the said square, the texture plus the shaders that enable the
     * rendering there
     */
    void init(const familyline::graphics::Window& win);

    /**
     * Render the cairo canvas to the gui texture
     */
    void renderToTexture();

    /**
     * Checks if an event mouse position hits a control or not.
     * This allows us to ignore events that do not belong to us, and pass them
     * to the handlers under it, such as the ones that handle game input
     *
     * If it did not hit any control, return false, else return true
     */
    bool checkIfEventHits(const familyline::input::HumanInputAction&);

    /**
     * Get the control that is at the specified pixel coordinate
     */
    std::optional<Control*> getControlAtPoint(int x, int y);

public:
    GUIManager(
        familyline::graphics::Window& win, unsigned width, unsigned height,
        familyline::input::InputManager& manager)
        : win_(win),
          width_(width),
          height_(height),
          canvas_(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height))
    {
        context_      = cairo_create(this->canvas_);
        this->init(win);

        root_control_ = std::make_unique<RootControl>(width, height);

        manager.addListenerHandler([&](familyline::input::HumanInputAction i) {
            if (this->checkIfEventHits(i)) {
                input_actions_.push(i);
                return true;
            }

            return false;
        });
    }

    void add(int x, int y, Control* control);
    void add(double x, double y, ControlPositioning cpos, Control* control);

    void remove(Control* control);

    void update();

    void render(unsigned int x, unsigned int y);

    /**
     * Receive an event and act on it
     *
     * (In the game, we will probably use input actions, not sdl events directly)
     */
    void receiveEvent();

    /**
     * Check if we have any pending callbacks
     *
     * If we have, run them
     *
     * Note that this function needs to be ran in the main thread, because the game loop
     * runs in one of those callbacks, the new game button. This can cause some race
     * conditions and crash the game.
     *
     */
    void runCallbacks();

    ~GUIManager();
};

}  // namespace familyline::graphics::gui
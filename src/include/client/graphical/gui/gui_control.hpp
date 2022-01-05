#pragma once

#include <array>
#include <functional>
#include <optional>
#include <string>

#include <variant>

#include <client/input/input_actions.hpp>

namespace familyline::graphics::gui {

enum class ButtonType {
  // The three greater and ubiquitous buttons
  Left,
  Right,
  Middle

  // The ones below exists almost exclusively on gaming mouses
  #if 0
  Button4,
  Button5,
  Button6,
  Button7,
  Button8,
  Button9,
  Button10
  #endif
};

/**
 * GUI glyph size, to notify the size of the font
 *
 * It is a normalized measurement, between 0 and 1.
 * If the control resizes, this value is invalid.
 */
struct GUIGlyphSize {
  double width;
  double height;
};

class GUIControl;

/**
 * Handler callback
 *
 * Since we will run a list of callbacks, we will need to store
 * a list of them, in a more or less generic way
 *
 * The first argument is the control from where the event
 * originated
 */
using FGUIEventCallback = std::function<void(GUIControl &)>;

/**
 * A callback to allow the control to send callbacks to the GUI Manager
 *
 * This allows us to send callbacks to the GUIManager without really
 * needing to have the GUIManager class instantiated
 */
using FGUICallbackRegister =
    std::function<void(FGUIEventCallback, GUIControl &)>;

/**
 * Text alignment
 */
enum HorizontalAlignment { Left, Center, Right };

enum class FontWeight { Regular, Bold };

/*
 * Some sort of appearance configuration for a control
 *
 * It defines things such as background, foreground and used font
 */
struct GUIAppearance {
  std::string font = "Arial";
  size_t fontsize = 12;
  std::array<double, 4> background = {0, 0, 0, 1};
    std::array<double, 4> foreground = {1, 1, 1, 1};
  HorizontalAlignment horizontalAlignment = HorizontalAlignment::Left;

  FontWeight weight = FontWeight::Regular;

  /// Margins, in pixels
  int marginX = 0;
  int marginY = 0;
};

/**
 * Gets a no-op callback register, just so that we do not call a null pointer
 * when we do not have it
 */
FGUICallbackRegister getDefaultCallbackRegister();
    
/**
 * An structure to allow the GUI manager to communicate some things
 * about the final layout of the control (to do things, for example,
 * like text selection) without breaking dependency
 */
struct GUIControlRenderInfo {

  /// GUIRenderer::getCodepointSize(...)
  std::function<std::optional<GUIGlyphSize>(char32_t, std::string_view, size_t,
                                            FontWeight)>
      getCodepointSize = [](auto, auto, auto, auto) { return std::nullopt; };

  /// GUIRenderer::setTextInputMode(...)
  std::function<void(bool)> setTextInputMode = [](auto) {};

  /**
   * The GUI manager that owns this control
   */
    void* gm;

    /**
     * A function to register the function for the GUI manager to runcallbacks*
     *
     * We need to run those functions in the GUI manager, because if we run them
     * in the control, if the handler remove the control (that can and *will*
     * happen), the game will segfault.
     */
    std::function<void(FGUIEventCallback, int)> registerEvent;
    /// ^ GUIRenderer::registerEvent(...);
};

/*
 * GUIControl
 *
 * The base class for every control
 * Everything that matters for a control is here.
 */
class GUIControl {
public:
  GUIControl(GUIControlRenderInfo render_info = {})
      : render_info(render_info) {}

  /// A textual way of describing the control
  /// If we were in Python, this would be its `__repr__` method
  ///
  /// Used *only* for debugging purposes.
  virtual std::string describe() const;

  /// Called when this control is resized or repositioned
  virtual void onResize(int nwidth, int nheight, int nx, int ny);

  int id() const { return id_; }

  int width() const { return width_; }
  int height() const { return height_; }
  int x() const { return x_; }
  int y() const { return y_; }

  void setMargins(int x, int y) {
    appearance_.marginX = x;
    appearance_.marginY = y;
  }

  /// Called if we need this control to autoresize, i.e, to change its
  /// size to whatever it seems fit.
  virtual void autoresize() {}

  /// Called when the parent need to update
  virtual void update(){};

  /// Called when an input is received
  virtual void receiveInput(const familyline::input::HumanInputAction &e) {}

  /// Returns true if the function needs to update its contents and/or redraw
  /// False if it does not
  bool dirty() const { return dirty_; }

  virtual ~GUIControl() {}

  /// Call this when you initialize the control
  /// (for example, when you add it to a layout, right before drawing it)
  void initialize(int id) {
    id_ = id;
    this->autoresize();
  }

  void setAppearance(GUIAppearance &a) {
    appearance_ = a;
    dirty_ = true;
  }

  std::optional<GUIControl *> &parent() { return parent_; }
  void setParent(std::optional<GUIControl *> p) { parent_ = p; }

  GUIAppearance appearance() const { return appearance_; };

  bool onFocus() const { return onFocus_; }

  /**
   * Focus enter and exit callbacks
   *
   * Remember that focus is *different* from hover.
   *
   * For a control go out of focus, we need to have
   *  - a tab change (you are operating on keyboard and uses tab to change
   *    focus.)
   *  - an active event must be delivered to another control (anything
   *    but MouseMoveEvent)
   *
   * A hover is the inverse: if a MouseMoveEvent is delivered to someone
   * else, the control is not being hovered anymore.
   */
  virtual void onFocusEnter() { onFocus_ = true; }
  virtual void onFocusExit() { onFocus_ = false; }

  virtual void setEventCallbackRegisterFunction(FGUICallbackRegister r) {
    cb_register_fn_ = r;
  }

private:
  int id_ = -1; /// The control's ID, aka how we must identify it?

protected:
  /// The control width and height, in absolute pixels
  int width_ = 0;
  int height_ = 0;

  /// The control X and Y position, in relation to its parent, and in pixel
  /// units.
  int x_ = 0;
  int y_ = 0;

  GUIControlRenderInfo render_info;

  bool onFocus_ = false;

  bool dirty_ = true;

  GUIAppearance appearance_;

  FGUICallbackRegister cb_register_fn_ = getDefaultCallbackRegister();

  /// This control's parent.
  ///
  /// An optional, to differentiate no parent (a nullopt) from
  /// an invalid parent (a NULLPTR)
  std::optional<GUIControl *> parent_ = std::nullopt;
};
}

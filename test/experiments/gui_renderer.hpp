#pragma once

#include <cstdint>
#include <string>

#include "gui.hpp"

/**
 * ControlPaintData
 *
 * Just an opaque class filled by the GUIControlPainter.
 * It has two methods: one to get the type of the framebuffer data, and
 * other to get the framebuffer data itself
 *
 * The type is not an enum, but since the renderer and the control painter
 * are usually paired together, one should know what the other supports.
 * It _might_ be possible to make everyone interchangeable, but this is
 * not what we want here
 *
 * The control paint data stores painting data for individual controls,
 * and they might be able to be combined.
 */
class ControlPaintData {
public:
  virtual std::string format() const = 0;
  virtual uint8_t *data() const = 0;

  virtual ~ControlPaintData() {}
};

/**
 * The GUIControlPainter
 *
 * Receives a control, returns a framebuffer with its data
 * The control painter might (and is encouraged to) cache contents
 *
 * The GUIManager will pass windows to it.
 * It is the painter's job to determine how it will treat the controls
 * inside the window
 *
 * We split the painter from the control so we can use other painting
 * APIs outside Cairo, and even no API, just to test the renderer's
 * behavior, and test the GUI itself
 */
class GUIControlPainter {
public:
  virtual std::unique_ptr<ControlPaintData> drawWindow(GUIWindow &w) = 0;

  /// This is here just as an helper, the renderer will not use this
  /// function directly
  virtual std::unique_ptr<ControlPaintData> drawControl(GUIControl &c) = 0;

  /// If you implement caching, you must implement this function, because
  /// some things (like window resizing) will need a full renderer of the
  /// window, and so the cache must be invalidated.
  virtual void invalidateCache() {}

  virtual ~GUIControlPainter(){};
};

/**
 * GUIRenderer
 *
 * This class will handle control rendering
 * It will take the data generated by the GUIControlPainter and put it
 * into a framebuffer
 * The Window class (from Familyline) will get this framebuffer and render
 * it to the screen
 */
class GUIRenderer {

public:
  /// Update the rendered content
  virtual void update(const std::vector<ControlPaintData *> &data) = 0;

  /// Render the data
  virtual void render() = 0;

  virtual std::unique_ptr<GUIControlPainter> createPainter() = 0;

  /**
   * For some controls, like the textbox, we need to know the exact
   * measurement of a certain character, so we can know where the cursor will
   * start selecting and where the cursor will be when we click inside of
   * the control.
   *
   * To do this, we need to know the size of each letter.
   * The 'letter', in this case, is an Unicode (really utf-32, but we can
   * assume utf-32 = unicode for a long time now) codepoint.
   *
   * We also need to provide font name, size and weight
   *
   * Return the codepoint size information, if found.
   * If not, return std::nullopt
   */
  virtual std::optional<GUIGlyphSize>
  getCodepointSize(char32_t codepoint, std::string_view fontName,
                   size_t fontSize, FontWeight weight) = 0;

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
  virtual void setTextInputMode(bool) = 0;

  /// The GUIManager will call this when it receives a resize event
  virtual void onResize(int width, int height) = 0;

  virtual ~GUIRenderer() {}
};

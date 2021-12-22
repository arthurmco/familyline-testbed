#pragma once

#include "gui_renderer.hpp"


class ConsoleControlPaintData : public ControlPaintData {
public:
  ConsoleControlPaintData(std::string text) : text_(text) {}

  virtual std::string format() const { return "console"; }
  virtual uint8_t *data() const { return (uint8_t *)text_.c_str(); }

  std::string text_;
};

//// Mock renderer just to test in the console

class ConsoleRenderer : public GUIRenderer {
public:
  /// Update the rendered content
  virtual void update(const std::vector<ControlPaintData *> &data);

  /// Render the data
  virtual void render();

  virtual void onResize(int width, int height);

  virtual std::optional<GUIGlyphSize>
  getCodepointSize(char32_t codepoint, std::string_view fontName,
                   size_t fontSize, FontWeight weight);

  virtual std::unique_ptr<GUIControlPainter> createPainter();

  int getCharHeightFor(int height) const;
  int getCharWidthFor(int width) const;

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

  virtual ~ConsoleRenderer() {}

private:
  std::vector<ControlPaintData *> data_;

  int charWidth_ = 110;
  int charHeight_ = 40;

  int screenWidth_ = 1;
  int screenHeight_ = 1;
};

class ConsoleControlPainter : public GUIControlPainter {
public:
  ConsoleControlPainter(ConsoleRenderer &cr) : cr_(cr) {}

  virtual std::unique_ptr<ControlPaintData> drawWindow(GUIWindow &w);
  virtual std::unique_ptr<ControlPaintData> drawControl(GUIControl &c);

private:
  ConsoleRenderer &cr_;

  std::string drawLabel(GUILabel &l, int cx, int cy, int ch, int cw);
  std::string drawTextbox(GUITextbox &l, int cx, int cy, int ch, int cw);
  std::string drawBox(GUIBox &b);
  std::string drawButton(GUIButton &btn, int cx, int cy, int ch, int cw);
};

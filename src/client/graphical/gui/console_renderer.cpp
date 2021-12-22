#include "console_renderer.hpp"

#include <algorithm>
#include <cstdio>

#include "gui_control.hpp"

/// Update the rendered content
void ConsoleRenderer::update(const std::vector<ControlPaintData*>& data)
{
  data_ = data;
}

std::optional<GUIGlyphSize>
ConsoleRenderer::getCodepointSize(char32_t codepoint, std::string_view fontName,
                                  size_t fontSize, FontWeight weight)
{
    // Ignore control characters
  if (codepoint < 32)
      return std::optional<GUIGlyphSize>(GUIGlyphSize{
              .width=0,
              .height=0
          });

  return std::optional<GUIGlyphSize>(GUIGlyphSize{
          .width=1.0*screenWidth_/charWidth_,
          .height=1.0*screenHeight_/charHeight_,
      });

  
}

void ConsoleRenderer::render()
{
  printf("\033[2J");
  for (int y = 0; y < charHeight_; y++) {
    printf("%02d\n", y);
  }

  for (auto* c : data_) {
    printf("%s", (char*)c->data());
  }

}

void ConsoleRenderer::onResize(int width, int height)
{
  screenWidth_ = width;
  screenHeight_ = height;
}

std::unique_ptr<GUIControlPainter> ConsoleRenderer::createPainter() {
  return std::make_unique<ConsoleControlPainter>(*this);
}


////  CW    x                        CW*w
//// --- = ---   = CW*w = SW*x = x = ----
////  SW    w                         SW
////

int ConsoleRenderer::getCharHeightFor(int height) const
{
  return (charHeight_ * height) / screenHeight_;
}

int ConsoleRenderer::getCharWidthFor(int width) const
{
  return (charWidth_ * width) / screenWidth_;
}


//////
//////
//////

#include <fmt/format.h>

static std::string save_cursor_position() { return("\033\x37"); }
static std::string restore_cursor_position() { return("\033\x38"); }

static std::string move_to(int x, int y)
{
  return fmt::format("\033[{};{}H", y+1, x+1);
}

static std::string move_to_relative(int x, int y)
{
  std::string xcmd = "";
  std::string ycmd = "";

  if (x > 0) {
    xcmd = fmt::format("\033[{}C", x);
  } else if (x < 0) {
    xcmd = fmt::format("\033[{}D", -x);
  }

  if (y > 0) {
    ycmd = fmt::format("\033[{}B", y);
  } else if (y < 0) {
    ycmd = fmt::format("\033[{}A", -y);
  }

  return fmt::format("{}{}", xcmd, ycmd);
}

static std::string change_foreground(double r, double g, double b) {
    return fmt::format("\033[38;2;{};{};{}m", int(r*255), int(g*255), int(b*255));
}

static std::string change_background(double r, double g, double b) {
    return fmt::format("\033[48;2;{};{};{}m", int(r*255), int(g*255), int(b*255));
}

static std::string reset_color() {
  return fmt::format("\033[0m");
}


std::string ConsoleControlPainter::drawBox(GUIBox& b)
{
  auto charh = cr_.getCharHeightFor(b.height());
  auto charw = cr_.getCharWidthFor(b.width());

  auto charx = cr_.getCharWidthFor(b.x());
  auto chary = cr_.getCharHeightFor(b.y());

  auto scontent = move_to(charx, chary);

  for (auto* control : b) {
    auto res = this->drawControl(*control);
    scontent += ((ConsoleControlPaintData*)res.get())->text_;
  }

  scontent += move_to(0, charh);
  return scontent;
}


std::unique_ptr<ControlPaintData> ConsoleControlPainter::drawWindow(GUIWindow& w)
{
    return std::make_unique<ConsoleControlPaintData>(drawBox(w.box()));
}


std::string
ConsoleControlPainter::drawButton(GUIButton& btn, int cx, int cy, int ch, int cw)
{
    auto scontent = save_cursor_position();

    auto appearance = btn.appearance();    
    auto [br, bg, bb, ba] = appearance.background;
//    auto [fr, fg, fb, fa] = appearance.foreground;
    scontent += change_background(br, bg, bb); // + change_foreground(fr, fg, fb);


    auto rbg = change_background(br, bg, bb);
    for (auto y = cy; y < cy+ch; y++) {
        scontent += move_to(cx, y);
        for (auto x = cx; x < cx+cw; x++) {
    
            std::string s = " ";

            // check here for a list of them
            /// https://en.wikipedia.org/wiki/Box-drawing_character
            
            // draw box characters
            if (y == cy || y == cy+ch-1)
                s = change_background(1.0, 1.0, 1.0) + " " + rbg;
            else if (x == cx || x == cx+cw-1)
                s = change_background(1.0, 1.0, 1.0) + " " + rbg;

            if (y == cy+ch-1 && x == cx)
                s = change_background(1.0, 1.0, 1.0) + " " + rbg;
            else if (y == cy+ch-1 && x == cx+cw-1)
                s = change_background(1.0, 1.0, 1.0) + " " + rbg;
            else if (y == cy && x == cx)
                s = change_background(1.0, 1.0, 1.0) + " " + rbg;
            else if (y == cy && x == cx+cw-1)
                s = change_background(1.0, 1.0, 1.0) + " " + rbg;
            
            scontent += s;
        }
    }

    if (ch > 3 && cw > 3)
        scontent += drawLabel(btn.getInnerLabel(), cx+2, cy+2, ch-3, cw-3);

    scontent += restore_cursor_position();
    return scontent;
}

std::string ConsoleControlPainter::drawTextbox(GUITextbox &text, int cx, int cy, int ch, int cw)
{
    auto scontent = save_cursor_position();

    auto appearance = text.appearance();    
    auto [br, bg, bb, ba] = appearance.background;

    
    if (text.onFocus()) {
        br = std::min(br+0.2, 1.0);
        bg = std::min(bg+0.2, 1.0);
        bb = std::min(bb+0.2, 1.0);
        ba = std::min(ba+0.2, 1.0);
    }

    auto [fr, fg, fb, fa] = appearance.foreground;
    scontent += change_background(br, bg, bb) + change_foreground(fr, fg, fb);


    for (auto y = cy; y < cy+ch; y++) {
        scontent += move_to(cx, y);
        for (auto x = cx; x < cx+cw; x++) {
    
            std::string s = " ";

            // check here for a list of them
            /// https://en.wikipedia.org/wiki/Box-drawing_character
            
            // draw box characters
            if (y == cy || y == cy+ch-1)
                s = "\u2500";
            else if (x == cx || x == cx+cw-1)
                s = "\u2502";

            if (y == cy+ch-1 && x == cx)
                s = "\u2514";
            else if (y == cy+ch-1 && x == cx+cw-1)
                s = "\u2518";
            else if (y == cy && x == cx)
                s = "\u250c";
            else if (y == cy && x == cx+cw-1)
                s = "\u2510";
            
            scontent += s;
        }
    }


    if (ch > 2 && cw > 2) {
        scontent += move_to(cx+1, cy+1);
        auto [begin, sel, end] = text.getTextAsSelection(true);

        scontent += begin;
        scontent += change_background(fr, fg, fb) + change_foreground(br, bg, bb);
        scontent += sel;
        scontent += change_background(br, bg, bb) + change_foreground(fr, fg, fb);
        scontent += end;
        
    }

    scontent += restore_cursor_position();
    return scontent;    
}


std::string
ConsoleControlPainter::drawLabel(GUILabel& l, int cx, int cy, int ch, int cw)
{
  auto tstr = l.text().substr(0, cw);
  auto len = tstr.size();
  auto appearance = l.appearance();

  auto xoff = 0;
  auto yoff = (ch/2) - 1;

  switch (appearance.horizontalAlignment) {
  case HorizontalAlignment::Left: xoff = 0; break;
  case HorizontalAlignment::Center: xoff = (cw/2) - (len/2);; break;
  case HorizontalAlignment::Right: xoff = std::max(size_t(0), cw-len-1); break;
  }

  std::string scontent = "";

  auto fill_label = [&]{
      std::string ret = ""; //save_cursor_position();
      ret += move_to(cx, cy);
      for (auto y = 0; y < ch; y++) {
          for (auto x = 0; x < cw; x++) {
              ret += ' ';
          }

          ret += move_to_relative(-cw, 1);
      }
      ret += move_to(cx, cy);
      return ret;
  };

  auto [br, bg, bb, ba] = appearance.background;
  auto [fr, fg, fb, fa] = appearance.foreground;

  if (l.onFocus()) {
      br = std::min(br+0.2, 1.0);
      bg = std::min(bg+0.2, 1.0);
      bb = std::min(bb+0.2, 1.0);
      ba = std::min(ba+0.2, 1.0);
  }

  if (ba > 0) {
      scontent += change_background(br, bg, bb) + fill_label();
  } else {
      scontent += change_background(0, 0, 0) + fill_label();
  }

  if (fa > 0) {
      scontent += change_foreground(fr, fg, fb);
  }

  scontent += move_to_relative(xoff, yoff) + tstr;
  if (fa > 0 || ba > 0) {
      scontent += reset_color();
  }

  return scontent;
}

std::unique_ptr<ControlPaintData> ConsoleControlPainter::drawControl(GUIControl& c)
{
  auto charh = cr_.getCharHeightFor(c.height());
  auto charw = cr_.getCharWidthFor(c.width());

  auto charx = cr_.getCharWidthFor(c.x());
  auto chary = cr_.getCharHeightFor(c.y());

  if (charx + charw < 0 || chary + charh < 0)
    return std::make_unique<ConsoleControlPaintData>("");

  auto scontent = save_cursor_position();
  scontent += move_to_relative(charx, chary);

  // Choose between the control types
  if (auto lbl = dynamic_cast<GUILabel*>(&c); lbl) {
    scontent += drawLabel(*lbl, charx, chary, charh, charw);
  } else if (auto btn = dynamic_cast<GUIButton*>(&c); btn) {
    scontent += drawButton(*btn, charx, chary, charh, charw);
  } else if (auto text = dynamic_cast<GUITextbox*>(&c); text) {
    scontent += drawTextbox(*text, charx, chary, charh, charw);
  } else if (auto box = dynamic_cast<GUIBox*>(&c); box) {
    scontent += drawBox(*box);
  } else {
    char content[64];
    scontent += content;
  }

  scontent += restore_cursor_position();

  return std::make_unique<ConsoleControlPaintData>(scontent);
}

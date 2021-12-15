#include <cstdio>
#include <cstdlib>

#include "gui.hpp"
#include "gui_manager.hpp"
//#include "console_renderer.hpp"
#include "test_renderer.hpp"

#include <memory>
#include <yaml-cpp/yaml.h>

////////

#include <fmt/format.h>

enum class LogType { Debug, Info, Warning, Error, Fatal };

/// A new log function, using libfmt and variadic templates!
template <typename... Args>
void log(const std::string tag, const LogType level, const std::string fmt,
         Args... args) {
  int time = 0;

  std::string strtype = "";
  switch (level) {
  case LogType::Debug:
    strtype = "";
    break;
  case LogType::Info:
    strtype = " [INFO]";
    break;
  case LogType::Warning:
    strtype = " [WARN]";
    break;
  case LogType::Error:
    strtype = " [ERROR]";
    break;
  case LogType::Fatal:
    strtype = " [FATAL]";
    break;
  }

  std::string data = fmt::format(fmt, args...);
  std::string ret = fmt::format("[{:8d}] {}:{} {}", time, tag, strtype, data);

  auto fd = stdout;

  fprintf(fd, "%s\r\n", ret.c_str());
  fflush(fd);
}

////////////
///////////
/////////////
///////////////

/**
 * TODO:
 *   - Add control margin, foreground and background, font weight change,
 *     text alignment and min/max width sizes (proportional or fixed)
 *   X Add tabbing
 *   ? Add a button control (and click/hover/focus events)
 *     (the hover event is impossible without mouse, maybe when we add it to the
 *     game)
 *   ? Add a textbox control
 *   - Add an image control (will search for assets instead of fixed paths?)
 *   - Add (or improve) multi-window support
 *   X Add a testing GUI renderer
 *   - Make possible to create menus using scheme
 *
 * And please split the files
 */

#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include <fmt/core.h>

struct termios orig_termios;

void reset_terminal_mode() { tcsetattr(0, TCSANOW, &orig_termios); }

void set_nonblocking_terminal_mode() {
  struct termios new_termios;

  /* take two copies - one for now, one for later */
  tcgetattr(0, &orig_termios);
  memcpy(&new_termios, &orig_termios, sizeof(new_termios));

  /* register cleanup handler, and set the new terminal mode */
  atexit(reset_terminal_mode);
  cfmakeraw(&new_termios);
  tcsetattr(0, TCSANOW, &new_termios);
}

int main() {
//  sleep(1);

  auto theme = std::make_unique<GUITheme>();
  theme->loadFile("theme.yaml");

  // Initialize the locale so that string encode conversions work.
  std::setlocale(LC_ALL, "");
  
//  set_nonblocking_terminal_mode();

  auto r = std::make_unique<TestRenderer>();
  GUIManager gm{std::move(r)};
  gm.theme = std::move(theme);
  
  GUIWindow &w = gm.createWindow<FlexLayout<false>>();
  w.onResize(800, 800, 0, 0);
  gm.onResize(800, 800);

  FlexLayout<true> blayout;
  GUILabel &l2 =
      (GUILabel &)w.box().add(gm.createControl<GUILabel>("Mama denovo"));
  GUIBox &b = (GUIBox &)w.box().add(gm.createControl<GUIBox>(blayout));

  GUILabel &l =
      (GUILabel &)b.add(gm.createControl<GUILabel>("A gente só tava aquecendo aí, Daivis"));
  GUILabel &l1 =
      (GUILabel &)b.add(gm.createNamedControl<GUILabel>("maiegui", "Mama meu ovo ali"));

  TestRenderer& tr = (TestRenderer&)gm.getRenderer();
  
  TestControlPaintData* l1t = tr.query(l1.id());
  fprintf(stderr, "%s\n", (l1t) ? "found" : "not found");
  
  gm.update();
  gm.render();

  l1t = tr.query(l1.id());
  fprintf(stderr, "%s\n", (l1t) ? "found" : "not found");

  if (l1t)
      fprintf(stderr, "%s\n", ((GUILabel&)l1t->control).text().c_str());
  
//  auto a = l1.appearance();
//  a.background = {0.2, 0, 0, 1}; // red
//  a.foreground = {0, 1, 1, 1};   // cyan
//  l1.setAppearance(a);

  

  /*
  GUIButton &bt = (GUIButton &)w.box().add(
      gm.createControl<GUIButton>("Clica no butão", [&](GUIControl &c) {
        ((GUIButton &)c).setText("Vc clicou no butão");
        auto g = gm.getControl<GUILabel>("maiegui");
        if (g)
            g->setText("AHHH tu deu pra ele, né?");

      }));


  for (int i = 0; i < 15; i++) {
    gm.update();
    gm.runEvents();
    gm.render();
    fflush(stdout);
    usleep(200000);
  }

  GUITextbox &t = (GUITextbox &)w.box().add(
      gm.createControl<GUITextbox>("my textbox áácçée$?ðđŋħ test "));
  GUILabel &l3 = (GUILabel &)w.box().add(
      gm.createControl<GUILabel>("Mama depois do update()"));
  gm.showWindow(w);

  gm.update();
  gm.update();

  gm.runEvents();
  gm.render();

  for (int i = 0; i < 100; i++) {
    gm.update();
    gm.runEvents();
    gm.render();
    fflush(stdout);
    usleep(50000);
  }

  log("guitest", LogType::Info, "{}", w.describe());
  log("guitest", LogType::Info, "{}", w.box().describe());
  log("guitest", LogType::Info, "{}", b.describe());
  log("guitest", LogType::Info, "{} {}", bt.id(), bt.describe());
  log("guitest", LogType::Info, "{}", l1.describe());
  log("guitest", LogType::Info, "{}", l.describe());
  log("guitest", LogType::Info, "{}", t.describe());
  log("guitest", LogType::Info, "{} {}", l2.id(), l2.describe());
  log("guitest", LogType::Info, "{}", l3.describe());
  */
}

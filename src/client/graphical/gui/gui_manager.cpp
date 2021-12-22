#include <client/graphical/gui/gui_manager.hpp>
#include <client/graphical/gui/gui_control.hpp>
#include <cstdint>
#include <fmt/core.h>
#include <functional>
#include <yaml-cpp/node/parse.h>

using namespace familyline::graphics::gui;

void GUITheme::loadFile(std::string path)
{
    root_node_ = YAML::LoadFile(path);

    fmt::print("Name: {}\n", root_node_["name"].as<std::string>());
    fmt::print("Description: {}\n", root_node_["description"].as<std::string>());
    
    file_loaded_ = true;
}

std::optional<GUIAppearance> GUITheme::getAppearanceFor(GUIControl* control) const
{
    if (!file_loaded_)
        return std::nullopt;

    
    if (auto lbl = dynamic_cast<GUILabel*>(control); lbl) {
        return getAppearanceByControlType("label");
    } else if (auto btn = dynamic_cast<GUIButton*>(control); btn) {
        return getAppearanceByControlType("button");
    } else if (auto text = dynamic_cast<GUITextbox*>(control); text) {
        return getAppearanceByControlType("textbox");
    } else if (auto box = dynamic_cast<GUIBox*>(control); box) {
        return getAppearanceByControlType("box");
    } else {
        return std::nullopt;
    }

}


std::optional<GUIAppearance> GUITheme::getAppearanceByControlType(std::string_view ctype) const
{    
    auto appearance = root_node_["controls"][(const char*)ctype.data()];

    if (!appearance) {
        return std::nullopt;
    }

    auto str2color = [](std::string value, std::array<double, 4> defaultv) {
        if (!value.starts_with("#"))
            return defaultv;

        auto cvalue = std::string_view{value};
        cvalue.remove_prefix(1);
        
        uint32_t r = 0x0, g = 0x0, b = 0x0, a = 0xff;
        if (value.size() > 7) {
            sscanf(cvalue.data(), "%02x%02x%02x%02x",
                   &r, &g, &b, &a);
        } else {
            sscanf(cvalue.data(), "%02x%02x%02x",
                   &r, &g, &b);            
        }

        return std::array<double, 4>{r/255.0, g/255.0, b/255.0, a/255.0};
    };
    
    GUIAppearance gapp;
    if (appearance["foreground"])
        gapp.foreground = str2color(appearance["foreground"].as<std::string>(), {0, 0, 0, 1.0});
    
    if (appearance["background"])
        gapp.background = str2color(appearance["background"].as<std::string>(), {0, 0, 0, 1.0});

    if (appearance["font_family"])
        gapp.font = appearance["font_family"].as<std::string>();

    if (appearance["font_size"])
        gapp.fontsize = (size_t)appearance["font_size"].as<uint32_t>();

    printf("\tforeground: %s -> %.2f %.2f %.2f %.2f\n",
           appearance["foreground"].as<std::string>().c_str(),
           gapp.foreground[0], gapp.foreground[1],
           gapp.foreground[2], gapp.foreground[3]);
    
    /*
    if (appearance["font_weight"])
        gapp.weight = appearance["font_weight"].as();
    */
    
    return std::make_optional(gapp);
}


void GUIManager::showWindow(GUIWindow &w) {
  auto window =
      std::find_if(windows_.begin(), windows_.end(),
                   [&](WindowInfo &wi) { return wi.window->id() == w.id(); });

  if (window == windows_.end())
    return;

  windows_.front().zIndex--;
  window->zIndex = shown_zindex_;
  window->window->setEventCallbackRegisterFunction(
      std::bind(&GUIManager::pushEvent, this, std::placeholders::_1, std::placeholders::_2 ));
  this->sortWindows();
}

void GUIManager::closeWindow(GUIWindow &w) {
  auto window =
      std::find_if(windows_.begin(), windows_.end(),
                   [&](WindowInfo &wi) { return wi.window->id() == w.id(); });

  if (window == windows_.end())
    return;

  window->zIndex = -1;
  window->window->setEventCallbackRegisterFunction(getDefaultCallbackRegister());
  
  if (windows_.size() >= 2) {
    windows_[1].zIndex = shown_zindex_;
    windows_[1].window->setEventCallbackRegisterFunction(
      std::bind(&GUIManager::pushEvent, this, std::placeholders::_1, std::placeholders::_2 ));
  }

  this->sortWindows();
}

void GUIManager::sortWindows() {
  std::sort(windows_.begin(), windows_.end(),
            [](const WindowInfo &a, const WindowInfo &b) {
              return a.zIndex > b.zIndex;
            });

  printf("Top window is %s \n", windows_.front().window->describe().c_str());
}

/// Called when we receive a window resize event
///
/// Usually, you will resize the windows proportionally, but, for now,
/// we will assume that all windows are fullscreen
void GUIManager::onResize(int width, int height) {
  std::for_each(windows_.begin(), windows_.end(),
                [width, height](WindowInfo &w) {
                  int relwidth = width;
                  int relheight = height;
                  int relx = 0;
                  int rely = 0;

                  w.window->onResize(relwidth, relheight, relx, rely);
                });

  renderer_->onResize(width, height);
}

/**
 * Run the event handlers
 */
void GUIManager::runEvents() {
  if (events_.empty())
    return;

  auto &event = events_.front();
  event.cb(event.control);
  events_.pop();
}

void GUIManager::update() {
  // Forward inputs to the front window
  // TODO: forward to the other windows if the front window is
  //       not in focus (if the event is a key event), or if
  //       the front window is not under the cursor (if it is
  //       a mouse event)
  this->listenInputs();
  for (auto &w : windows_) {
    while (!inputs_.empty()) {
      w.window->receiveInput(inputs_.front());
      inputs_.pop();
    }

    break;
  }

  // Draw the window contents, backwards, because the front window is the last
  // window to be drawn
  std::for_each(windows_.rbegin(), windows_.rend(), [this](WindowInfo &w) {
    if (w.window->dirty()) {
      w.window->update();
      w.paint_data = this->painter_->drawWindow(*w.window.get());
    }
  });

  renderer_paint_data_.clear();
  std::transform(windows_.rbegin(), windows_.rend(),
                 std::back_inserter(renderer_paint_data_),
                 [](const WindowInfo &w) { return w.paint_data.get(); });

  renderer_->update(renderer_paint_data_);
}

void GUIManager::render() { renderer_->render(); }

#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
int kbhit() {
  struct timeval tv = {0L, 0L};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(0, &fds);
  return select(1, &fds, NULL, NULL, &tv);
}

int getch() {
  int r;
  unsigned char c = 0xff;
  if ((r = read(0, &c, sizeof(c))) < 0) {
    return r;
  } else {
    return c;
  }
}

/**
 * Listen for inputs, add them into the event input queue
 */
void GUIManager::listenInputs() {
  if (kbhit()) {
      auto ch = (char)getch();
      auto ev = KeyEvent{
          .key = ch,
          .ctrl = false,
          .alt = false,
          .shift = false,
          .isPressing = true,
          .isReleasing = false
      };

      inputs_.push(ev);

      ev = KeyEvent{
          .key = ch,
          .ctrl = false,
          .alt = false,
          .shift = false,
          .isPressing = false,
          .isReleasing = true
      };

      inputs_.push(ev);
  }
}

#pragma once

#include <cstdio>
#include <cstdlib>

#include <cstring>
#include <string>
#include <vector>

#include <algorithm>
#include <functional>
#include <memory>
#include <optional>

#include <client/graphical/gui/gui_box.hpp>
#include <client/graphical/gui/gui_control.hpp>
#include <client/graphical/gui/gui_layout.hpp>

namespace familyline::graphics::gui {

/**
 * GUIWindow
 *
 * The window
 * Every widget lives here.
 * To position the widgets inside the view, you must need a BaseLayout
 */
class GUIWindow : public GUIControl {
public:
  GUIWindow(BaseLayout &layout, GUIControlRenderInfo i = {})
      : GUIControl(i), box_(layout) {
      box_.initialize(this->id());
  }

  virtual std::string describe() const;

  /// Called when this control is resized or repositioned
  virtual void onResize(int nwidth, int nheight, int nx, int ny);

  /// Called when the parent need to update
  virtual void update();

  virtual void receiveInput(const familyline::input::HumanInputAction &e);

  GUIBox &box() { return box_; }

  virtual void
  setEventCallbackRegisterFunction(FGUICallbackRegister r) override {
    cb_register_fn_ = r;
    box_.setEventCallbackRegisterFunction(r);
  }

private:
  bool inner_box_ = false;
  bool last_inner_box_ = true;
  GUIBox box_;
};

}

#include <client/graphical/gui/gui.hpp>
#include <client/graphical/gui/gui_manager.hpp>
#include <string>

using namespace familyline::graphics::gui;


std::string GUIWindow::describe() const {
  char v[128] = {};
  sprintf(v, "GUIWindow (id %08x, size %d x %d)", this->id(), width_, height_);
  return std::string{v};
}

void GUIWindow::onResize(int nwidth, int nheight, int nx, int ny) {
  width_ = nwidth;
  height_ = nheight;
  box_.onResize(nwidth, nheight, nx, ny);
}

/// Called when the parent need to update
void GUIWindow::update() {
  box_.update();
  dirty_ = true;
};


void GUIWindow::receiveInput(const familyline::input::HumanInputAction &e) {
    using namespace familyline::input;

    if (auto *kev = std::get_if<KeyAction>(&e.type); kev) {
    if (kev->keycode == SDLK_TAB && kev->isPressed) {
        if (!inner_box_) {
            this->box_.onFocusExit();
        }
        inner_box_ = this->box_.forwardTabIndexEvent();
    }
  }

  this->box().receiveInput(e);
}

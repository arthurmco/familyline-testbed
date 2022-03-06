#include <client/graphical/gui/gui_box.hpp>
#include <client/graphical/gui/gui_manager.hpp>
#include <numeric>

using namespace familyline::graphics::gui;

/**
 * Set the minimum and maximum sizes of this box according to
 * our children
 */
void GUIBox::resetBoundarySizes()
{
    using bound_info = std::array<std::optional<unsigned>, 4>;

    bound_info info;
    for (GUIControl *control : controls_) {
        GUIAppearance a = control->appearance();

        if (a.minWidth) {
            info[0] = std::make_optional(std::max(info[0].value_or(1), *a.minWidth));
        }
        if (a.minHeight) {
            info[1] = std::make_optional(std::max(info[1].value_or(1), *a.minHeight));
        }

        if (a.maxWidth && width_ > 0) {
            info[2] = std::make_optional(std::min(info[2].value_or(width_), *a.maxWidth));
        }
        if (a.maxHeight && height_ > 0) {
            info[3] = std::make_optional(std::min(info[3].value_or(height_), *a.maxHeight));
        }
    }

    GUIAppearance a = this->appearance();
    a.minWidth      = info[0];
    a.minHeight     = info[1];
    a.maxWidth      = info[2];
    a.maxHeight     = info[3];
    this->setAppearance(a);
}

GUIControl &GUIBox::add(GUIControl *c)
{
    assert(c);
    static int nid = 0x10000;

    c->initialize(nid++);
    auto &ret = *c;
    controls_.push_back(c);

    if (auto flex = dynamic_cast<FlexLayout<true> *>(&layout_); flex) {
        flex->add(ret.id());
    }

    if (auto flex = dynamic_cast<FlexLayout<false> *>(&layout_); flex) {
        flex->add(ret.id());
    }

    ret.setParent(std::make_optional(this));
    ret.setEventCallbackRegisterFunction(cb_register_fn_);
    resetBoundarySizes();

    return ret;
}

void GUIBox::remove(GUIControl &c) { assert(false); }

/**
 * Get a certain control by its ID
 */
GUIControl *GUIBox::get(int id) const
{
    std::vector<GUIBox *> boxes;
    boxes.reserve(controls_.size());

    for (auto c : controls_) {
        if (c->id() == id) return c;

        // Store the box so they can be searched later.
        if (auto cbox = dynamic_cast<GUIBox *>(c); cbox) {
            boxes.push_back(cbox);
        }
    }

    for (auto b : boxes) {
        GUIControl *cson = b->get(id);
        if (cson) return cson;
    }

    return nullptr;
}

/// A textual way of describing the control
/// If we were in Python, this would be its `__repr__` method
///
/// Used *only* for debugging purposes.
std::string GUIBox::describe() const
{
    char v[96];
    sprintf(
        v, "GUIBox: size %d x %d, pos: %d, %d (%zu elements): [ ", width_, height_, x_, y_,
        controls_.size());

    std::string ret{v};

    for (auto c : controls_) ret += c->describe() + " ";

    ret += "]";
    return ret;
}

void GUIBox::setEventCallbackRegisterFunction(FGUICallbackRegister r)
{
    cb_register_fn_ = r;
    for (auto c : controls_) {
        c->setEventCallbackRegisterFunction(r);
    }
}

/// Called when this control is resized or repositioned
void GUIBox::onResize(int nwidth, int nheight, int nx, int ny)
{
    width_  = nwidth;
    height_ = nheight;
    x_      = nx;
    y_      = ny;

    layout_.resize(width_, height_, x_, y_);
    resetBoundarySizes();
    dirty_ = true;
}

void GUIBox::autoresize()
{
    std::for_each(controls_.begin(), controls_.end(), [](GUIControl *c) { c->autoresize(); });
}

/**
 * Called when the parent need to update
 *
 * It only updates when we are dirty or when our children are dirty
 */
void GUIBox::update()
{
    bool dirty = dirty_ || std::any_of(controls_.begin(), controls_.end(), [](GUIControl *c) {
                     return c->dirty();
                 });
    if (!dirty) return;

    layout_.update();
    for (auto &c : controls_) {
        if (c->dirty() || dirty_) c->update();
    }

    dirty_ = false;
};

void GUIBox::onDestroyChild(unsigned long id)
{
    for (GUIControl *c : controls_) {
        if (auto box = dynamic_cast<GUIBox *>(c); box) {
            this->forwardDestroyToChildBox(id, *box);
        }

        if (focused_index_ >= controls_.size()) {
            focused_index_ = -1;
        } else if (focused_index_ >= 0 && id == (*(controls_.begin() + focused_index_))->id()) {
            focused_index_ = -1;
        }

        if (last_focus_control_id == id) {
            last_focus_control_id = -1;
        }
    }

    auto r = std::remove_if(
        controls_.begin(), controls_.end(), [id](GUIControl *c) { return c->id() == id; });

    controls_.erase(r, controls_.end());
}

void GUIBox::receiveInput(const familyline::input::HumanInputAction &e)
{
    if (controls_.size() == 0) {
        return;
    }

    using namespace familyline::input;
    auto focus_control = controls_.begin();

    if (std::holds_alternative<MouseAction>(e.type)) {
        is_tab     = false;
        auto event = std::get<MouseAction>(e.type);

        auto mouse_focus =
            std::find_if(controls_.begin(), controls_.end(), [event](const GUIControl *c) {
                return (
                    event.screenX >= c->x() && event.screenX < c->x() + c->width() &&
                    event.screenY >= c->y() && event.screenY < c->y() + c->height());
            });

        if (mouse_focus == controls_.end()) return;

        focused_index_ = std::distance(controls_.begin(), mouse_focus);

    } else {
        if (is_tab) {
            if (focused_index_ < 0) return;
        }
    }

    if (focused_index_ < 0) return;

    std::advance(focus_control, focused_index_);

    GUIControl *last_focus_control =
        (last_focus_control_id >= 0)
            ? ((GUIManager *)render_info.gm)->getControl<GUIControl>(last_focus_control_id)
            : nullptr;

    if (last_focus_control != (*focus_control)) {
        if (last_focus_control) last_focus_control->onFocusExit();

        (*focus_control)->onFocusEnter();
    }

    (*focus_control)->receiveInput(e);

    if (focused_index_ >= 0)
        last_focus_control_id = (*focus_control)->id();
    else
        last_focus_control_id = -1;
}

/**
 * Forward the tab index event (aka the act of changing
 * control focus when you press TAB) to the next
 * element
 *
 * Returns true while we have elements to tab, false when we do not have any
 */
bool GUIBox::forwardTabIndexEvent()
{
    is_tab = true;
    if (tab_index_ < 0 || tab_index_ >= ssize_t(controls_.size())) {
        tab_index_ = 0;
    }

    auto tab_control      = controls_.begin();
    auto prev_tab_control = controls_.begin();

    std::advance(tab_control, tab_index_);

    focused_index_ = tab_index_;

    if (auto box = dynamic_cast<GUIBox *>(*tab_control); box) {
        inner_box_ = box->forwardTabIndexEvent();
    } else {
        (*tab_control)->onFocusEnter();
    }

    if (previous_tab_index_ >= 0 && previous_tab_index_ != tab_index_) {
        std::advance(prev_tab_control, previous_tab_index_);
        (*prev_tab_control)->onFocusExit();
    }

    previous_tab_index_ = tab_index_;

    if (tab_index_ + 1 >= ssize_t(controls_.size())) {
        return false;
    }

    if (!inner_box_) tab_index_++;

    return true;
}

void GUIBox::initLayout(BaseLayout &layout)
{
    layout.initialize(
        [this]() {
            std::vector<int> ids;
            std::transform(
                this->controls_.begin(), this->controls_.end(), std::back_inserter(ids),
                [](auto *c) { return c->id(); });
            return ids;
        },
        [this](int id) -> GUIControl * {
            auto v = std::find_if(this->controls_.begin(), this->controls_.end(), [id](auto *c) {
                return c->id() == id;
            });
            if (v == this->controls_.end()) return nullptr;

            return *v;
        });
}

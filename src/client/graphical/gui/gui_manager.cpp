#include <fmt/core.h>
#include <yaml-cpp/node/parse.h>

#include <client/graphical/gui/gui_control.hpp>
#include <client/graphical/gui/gui_manager.hpp>
#include <common/logger.hpp>
#include <cstdint>
#include <functional>
#include <ranges>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/reverse.hpp>

using namespace familyline::graphics::gui;

void GUITheme::loadFile(std::string path)
{
    root_node_ = YAML::LoadFile(path);

    fmt::print("Name: {}\n", root_node_["name"].as<std::string>());
    fmt::print("Description: {}\n", root_node_["description"].as<std::string>());

    file_loaded_ = true;
}

std::optional<GUIAppearance> GUITheme::getAppearanceFor(GUIControl *control) const
{
    if (!file_loaded_) return std::nullopt;

    if (auto lbl = dynamic_cast<GUILabel *>(control); lbl) {
        return getAppearanceByControlType("label");
    } else if (auto btn = dynamic_cast<GUIButton *>(control); btn) {
        return getAppearanceByControlType("button");
    } else if (auto text = dynamic_cast<GUITextbox *>(control); text) {
        return getAppearanceByControlType("textbox");
    } else if (auto box = dynamic_cast<GUIBox *>(control); box) {
        return getAppearanceByControlType("box");
    } else {
        return std::nullopt;
    }
}

std::optional<GUIAppearance> GUITheme::getAppearanceByControlType(std::string_view ctype) const
{
    auto appearance = root_node_["controls"][(const char *)ctype.data()];

    if (!appearance) {
        return std::nullopt;
    }

    auto str2color = [](std::string value, std::array<double, 4> defaultv) {
        if (!value.starts_with("#")) return defaultv;

        auto cvalue = std::string_view{value};
        cvalue.remove_prefix(1);

        uint32_t r = 0x0, g = 0x0, b = 0x0, a = 0xff;
        if (value.size() > 7) {
            sscanf(cvalue.data(), "%02x%02x%02x%02x", &r, &g, &b, &a);
        } else {
            sscanf(cvalue.data(), "%02x%02x%02x", &r, &g, &b);
        }

        return std::array<double, 4>{r / 255.0, g / 255.0, b / 255.0, a / 255.0};
    };

    GUIAppearance gapp;
    if (appearance["foreground"])
        gapp.foreground = str2color(appearance["foreground"].as<std::string>(), {0, 0, 0, 1.0});

    if (appearance["background"])
        gapp.background = str2color(appearance["background"].as<std::string>(), {0, 0, 0, 1.0});

    if (appearance["font_family"]) gapp.font = appearance["font_family"].as<std::string>();

    if (appearance["font_size"]) gapp.fontsize = (size_t)appearance["font_size"].as<uint32_t>();

    if (appearance["horizontal_alignment"]) {
        std::string alignment = appearance["horizontal_alignment"].as<std::string>();
        
        if (alignment == "left")
            gapp.horizontalAlignment = HorizontalAlignment::Left;
        else if (alignment == "center")
            gapp.horizontalAlignment = HorizontalAlignment::Center;
        else if (alignment == "right")
            gapp.horizontalAlignment = HorizontalAlignment::Right;
    }

    
    printf(
        "\tforeground: %s -> %.2f %.2f %.2f %.2f\n",
        appearance["foreground"].as<std::string>().c_str(), gapp.foreground[0], gapp.foreground[1],
        gapp.foreground[2], gapp.foreground[3]);

    /*
    if (appearance["font_weight"])
        gapp.weight = appearance["font_weight"].as();
    */

    return std::make_optional(gapp);
}

void GUIManager::showWindow(GUIWindow &w)
{
    auto window = std::find_if(windows_.begin(), windows_.end(), [&](WindowInfo &wi) {
        return wi.window->id() == w.id();
    });

    if (window == windows_.end()) return;

    window->zIndex = std::max(shown_zindex_, windows_.front().zIndex + 1);
    windows_.front().zIndex--;
    window->visible = true;
    window->window->setEventCallbackRegisterFunction(
        std::bind(&GUIManager::pushEvent, this, std::placeholders::_1, std::placeholders::_2));
    this->sortWindows();
}

void GUIManager::closeWindow(GUIWindow &w)
{
    auto window = std::find_if(windows_.begin(), windows_.end(), [&](WindowInfo &wi) {
        return wi.window->id() == w.id();
    });

    if (window == windows_.end()) return;

    window->zIndex = 0;
    window->window->setEventCallbackRegisterFunction(getDefaultCallbackRegister());
    window->visible = false;

    this->sortWindows();
}

void GUIManager::moveWindowToTop(GUIWindow &w)
{
    auto window = std::find_if(windows_.begin(), windows_.end(), [&](WindowInfo &wi) {
        return wi.window->id() == w.id();
    });

    if (window == windows_.end()) return;

    window->zIndex  = windows_.front().zIndex + 1;
    window->visible = true;

    this->sortWindows();
}

/**
 * Removes a window from the window list.
 */
void GUIManager::destroyWindow(std::string name)
{
    auto window = std::remove_if(windows_.begin(), windows_.end(), [&](const WindowInfo &wi) {
        return wi.name == name;
    });

    windows_.erase(window, windows_.end());
}

void GUIManager::sortWindows()
{
    std::sort(windows_.begin(), windows_.end(), [](const WindowInfo &a, const WindowInfo &b) {
        return a.zIndex > b.zIndex;
    });

    auto &log = LoggerService::getLogger();
    log->write(
        "gui-manager", LogType::Info, "Top window is {} ({})", windows_.front().window->describe(),
        windows_.front().name);
}

/// Called when we receive a window resize event
///
/// Usually, you will resize the windows proportionally, but, for now,
/// we will assume that all windows are fullscreen
void GUIManager::onResize(int width, int height)
{
    std::for_each(windows_.begin(), windows_.end(), [width, height](WindowInfo &w) {
        int relwidth  = width;
        int relheight = height;
        int relx      = 0;
        int rely      = 0;

        w.window->onResize(relwidth, relheight, relx, rely);
    });

    renderer_->onResize(width, height);
}

/**
 * Run the event handlers
 */
void GUIManager::runEvents()
{
    if (!events_.empty()) {
        auto &event = events_.front();
        event.cb(event.control);
        events_.pop();        
    }

    if (!callbacks_.empty()) {
        auto &[callback, controlID] = callbacks_.front();
        GUIControl* c = this->getControl<GUIControl>(controlID);

        if (c) {
            callback(*c);
        } else {
        
        }
    
        callbacks_.pop();
        
    }
    
}

GUIWindow *GUIManager::getWindow(std::string name)
{
    auto it = std::find_if(windows_.begin(), windows_.end(), [&name](const WindowInfo &wi) {
        return wi.window && wi.name == name;
    });

    if (it == windows_.end()) return nullptr;

    return it->window.get();
}

void GUIManager::update()
{
    // Forward inputs to the front window
    // TODO: forward to the other windows if the front window is
    //       not in focus (if the event is a key event), or if
    //       the front window is not under the cursor (if it is
    //       a mouse event)

    // Draw the window contents, backwards, because the front window is the last
    // window to be drawn
    std::for_each(windows_.rbegin(), windows_.rend(), [this](WindowInfo &w) {
        if (w.window->dirty() && w.visible) {
            w.window->update();
            w.paint_data = this->painter_->drawWindow(*w.window.get());
        }
    });

    renderer_paint_data_.clear();
    auto validwindows = windows_ |
        ranges::views::filter([](const WindowInfo &w) { return w.visible; }) |
        ranges::views::reverse;

    std::transform(
        validwindows.begin(), validwindows.end(), std::back_inserter(renderer_paint_data_),
        [](const WindowInfo &w) { return w.paint_data.get(); });

    renderer_->update(renderer_paint_data_);
}

void GUIManager::render() { renderer_->render(); }

unsigned lastX = 0, lastY = 0;
/**
 * Listen for inputs, add them into the event input queue
 */
bool GUIManager::listenInputs(familyline::input::HumanInputAction i)
{
    using namespace familyline::input;

    for (auto &w : windows_ | ranges::views::filter(
             [](const WindowInfo &wi) { return wi.visible; }) ) {
        if (std::holds_alternative<MouseAction>(i.type)) {
            auto event = std::get<MouseAction>(i.type);

            lastX = event.screenX;
            lastY = event.screenY;

            if (event.screenX >= w.window->x() &&
                event.screenX < (w.window->x() + w.window->width()) &&
                event.screenY >= w.window->y() &&
                event.screenY < (w.window->y() + w.window->height())) {
                w.window->receiveInput(i);
                return true;
            }

        } else if (std::holds_alternative<ClickAction>(i.type)) {
            auto event = std::get<ClickAction>(i.type);

            lastX = event.screenX;
            lastY = event.screenY;
            if (event.screenX >= w.window->x() &&
                event.screenX < (w.window->x() + w.window->width()) &&
                event.screenY >= w.window->y() &&
                event.screenY < (w.window->y() + w.window->height())) {
                fprintf(stderr, "%s %08x %08x\n", w.name.c_str(), w.window->id(), windows_.size());
                w.window->receiveInput(i);
                return true;
            }

        } else if (std::holds_alternative<GameExit>(i.type)) {
            return false;
        } else {
            if (lastX >= w.window->x() && lastX < (w.window->x() + w.window->width()) &&
                lastY >= w.window->y() && lastY < (w.window->y() + w.window->height())) {
                w.window->receiveInput(i);
                return true;
            }
        }
    }

    return false;
}

void GUIManager::registerEvent(FGUIEventCallback handler, int control_id)
{
    callbacks_.push(std::make_pair(handler, control_id));
}

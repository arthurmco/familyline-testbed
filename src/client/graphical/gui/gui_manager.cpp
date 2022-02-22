#include <fmt/core.h>
#include <yaml-cpp/node/parse.h>

#include <client/graphical/gui/gui_control.hpp>
#include <client/graphical/gui/gui_manager.hpp>
#include <common/logger.hpp>
#include <cstdint>
#include <functional>
#include <range/v3/all.hpp>

using namespace familyline::graphics::gui;

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
    auto window = std::remove_if(
        windows_.begin(), windows_.end(), [&](const WindowInfo &wi) { return wi.name == name; });

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
    while (!events_.empty()) {
        auto &event = events_.front();
        event.cb(event.control);
        events_.pop();
    }

    if (!callbacks_.empty()) {
        auto &[callback, controlID] = callbacks_.front();
        GUIControl *c               = this->getControl<GUIControl>(controlID);

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

void GUIManager::render()
{
    renderer_->debugWrite(debugOut_);
    renderer_->render();
}

unsigned lastX = 0, lastY = 0;
/**
 * Listen for inputs, add them into the event input queue
 */
bool GUIManager::listenInputs(familyline::input::HumanInputAction i)
{
    using namespace familyline::input;

    for (auto &w :
         windows_ | ranges::views::filter([](const WindowInfo &wi) { return wi.visible; })) {
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

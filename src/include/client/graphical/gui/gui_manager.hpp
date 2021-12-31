#pragma once

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>

#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

#include <client/graphical/gui/gui.hpp>
#include <client/graphical/gui/gui_control.hpp>
#include <client/graphical/gui/gui_renderer.hpp>

#include <client/input/input_service.hpp>

namespace familyline::graphics::gui {

/**
 * GUITheme
 *
 * Each control type has a predefined sets of attributes.
 * They are known as the control 'theme'
 *
 * You might think the game does not need to have such flexibility as themes,
 * but:  a) development time will be very reduced if we can choose colors
 * without recompiling and b) skins are a thing now.
 */
class GUITheme
{
public:
    GUITheme() {}

    void loadFile(std::string);

    std::optional<GUIAppearance> getAppearanceFor(GUIControl *control) const;

private:
    std::optional<GUIAppearance> getAppearanceByControlType(std::string_view) const;

    bool file_loaded_ = false;
    YAML::Node root_node_;
};

/**
 * GUIManager
 *
 * Has ownership of every control and every window, and has methods to
 * create them.
 * Also, you are able to query them from here.
 *
 * It also controls rendering of those controls, and controls input sending or
 * receiving from them
 */
class GUIManager
{
public:
    GUIManager(std::unique_ptr<GUIRenderer> renderer)
        : renderer_(std::move(renderer)), painter_(renderer_->createPainter())
    {
        render_info_ = GUIControlRenderInfo{
            .getCodepointSize = std::bind(
                &GUIManager::getCodepointSize, this, std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3, std::placeholders::_4),
            .setTextInputMode =
                std::bind(&GUIManager::setTextInputMode, this, std::placeholders::_1)};

        // Initialize the locale so that string encode conversions work.
        std::setlocale(LC_ALL, "");

        auto& im = familyline::input::InputService::getInputManager();
        im->addListenerHandler(std::bind(&GUIManager::listenInputs, this, std::placeholders::_1));
    }

    struct WindowInfo {
        std::unique_ptr<GUIWindow> window;
        std::unique_ptr<BaseLayout> layout;
        std::unique_ptr<ControlPaintData> paint_data;

        // The z-index
        // The higher it is, the most on top it will show
        int zIndex = 0;
    };

    struct EventInfo {
        FGUIEventCallback cb;
        GUIControl &control;
    };

    /**
     * Create a control of type T, in place
     * Also pass the arguments of said control into the constructor, more or less
     * like make_unique would do.
     *
     * We store the control in this GUIManager and return a pointer to it,
     * properly converted
     */
    template <typename Control, typename... Args>
    requires std::derived_from<Control, GUIControl> Control *createControl(Args &&...args)
    {
        auto ptr     = std::make_unique<Control>(args..., render_info_);
        Control *ret = (Control *)ptr.get();

        auto appearance = theme->getAppearanceFor(ret);
        if (appearance) ret->setAppearance(*appearance);

        controls_.push_back(std::move(ptr));

        return ret;
    }

    /**
     * Create a control of type T, in place
     * Same thing as the above, but define a name for the control
     *
     * Useful if you want to get the control later while inside a
     * handle, where the control reference might not exist anymore.
     */
    template <typename Control, typename... Args>
    Control *createNamedControl(std::string name, Args &&...args)
    {
        Control *c          = createControl<Control>(args...);
        name2control_[name] = c;

        return c;
    }

    template <typename Control>
    requires std::derived_from<Control, GUIControl> Control *getControl(std::string name)
    {
        if (!name2control_.contains(name)) {
            return nullptr;
        }

        return (Control *)name2control_[name];
    }

    /**
     * Creates a window, type parameterizing its layout, because we do not need to
     * worry about creating it ourselves.
     *
     * (We also need a way to pass parameters to future layouts, but a parameter
     * pack might be OK)
     */
    template <typename Layout>
    requires std::derived_from<Layout, BaseLayout>
    GUIWindow &createWindow()
    {
        static int winid = 1;
        auto layoutptr   = std::make_unique<Layout>();

        auto ptr       = std::make_unique<GUIWindow>(*layoutptr.get(), render_info_);
        GUIWindow *ret = ptr.get();
        ptr->initialize(winid);

        auto paintdata = painter_->drawWindow(*ret);

        windows_.push_back(WindowInfo{std::move(ptr), std::move(layoutptr), std::move(paintdata)});
        return *ret;
    }

    // https://www.amazon.com/High-Performance-Master-optimizing-functioning/dp/1839216549

    /// Called when we receive a window resize event
    ///
    /// Usually, you will resize the windows proportionally, but, for now,
    /// we will assume that all windows are fullscreen
    void onResize(int width, int height);

    /// The current theme for every control created here.
    std::unique_ptr<GUITheme> theme = std::make_unique<GUITheme>();

    std::optional<GUIGlyphSize> getCodepointSize(
        char32_t codepoint, std::string_view fontName, size_t fontSize, FontWeight weight)
    {
        return renderer_->getCodepointSize(codepoint, fontName, fontSize, weight);
    }

    void showWindow(GUIWindow &);
    void closeWindow(GUIWindow &);

    void update();
    void render();

    /**
     * Run the event handlers.
     *
     * One event will be ran for each call to this function.
     */
    void runEvents();

    /**
     * Listen for inputs, add them into the event input queue
     */
    bool listenInputs(familyline::input::HumanInputAction i);

    /**
     * Push the event to be ran the next time you call `runEvents()`
     */
    void pushEvent(FGUIEventCallback cb, GUIControl &control) { events_.emplace(cb, control); }

    GUIRenderer& getRenderer() { return *renderer_.get(); }
    
private:
    std::vector<std::unique_ptr<GUIControl>> controls_;
    std::vector<WindowInfo> windows_;
    
    std::map<std::string, GUIControl *> name2control_;

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
    void setTextInputMode(bool v) { renderer_->setTextInputMode(v); }

    GUIControlRenderInfo render_info_;

    std::unique_ptr<GUIRenderer> renderer_;
    std::unique_ptr<GUIControlPainter> painter_;

    std::vector<ControlPaintData *> renderer_paint_data_;

    std::queue<EventInfo> events_;

    int shown_zindex_ = 100;

    /// Sort windows by zIndex
    /// The higher the zIndex, the closer to the first element it is.
    void sortWindows();
};

}

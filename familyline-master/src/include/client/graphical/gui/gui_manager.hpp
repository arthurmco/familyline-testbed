#pragma once

#include <client/graphical/gui/gui.hpp>
#include <client/graphical/gui/gui_control.hpp>
#include <client/graphical/gui/gui_renderer.hpp>
#include <client/graphical/gui/theme.hpp>
#include <client/input/input_service.hpp>
#include <common/logic/script_environment.hpp>
#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

namespace familyline::graphics::gui
{
class GUIScriptRunner;

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
                std::bind(&GUIManager::setTextInputMode, this, std::placeholders::_1),
            .gm            = (void *)this,
            .registerEvent = std::bind(
                &GUIManager::registerEvent, this, std::placeholders::_1, std::placeholders::_2)};

        // Initialize the locale so that string encode conversions work.
        std::setlocale(LC_ALL, "");

        auto &im = familyline::input::InputService::getInputManager();
        im->addListenerHandler(std::bind(&GUIManager::listenInputs, this, std::placeholders::_1));
    }

    struct WindowInfo {
        std::string name;

        std::unique_ptr<GUIWindow> window;
        std::unique_ptr<BaseLayout> layout;
        std::unique_ptr<ControlPaintData> paint_data;

        bool visible = false;

        // The z-index
        // The higher it is, the most on top it will show
        int zIndex = 0;
    };

    struct EventInfo {
        FGUIEventCallback cb;
        GUIControl &control;
    };

    /**
     * Create a control of type Control, in place
     * Also pass the arguments of said control into the constructor, more or less
     * like make_unique would do.
     *
     * We store the control in this GUIManager and return a pointer to it,
     * properly converted
     */
    template <typename Control, typename... Args>
    requires std::derived_from<Control, GUIControl> Control *createControl(Args &&...args)
    {
        render_info_.gm = (void *)this;
        auto ptr        = std::make_unique<Control>(args..., render_info_);
        Control *ret    = (Control *)ptr.get();

        auto appearance = theme->getAppearanceFor(ret);
        if (appearance) ret->setAppearance(*appearance);

        controls_.push_back(std::move(ptr));

        return ret;
    }

    /**
     * Create a control of type Control, in place
     * Same thing as `createControl`, but define a name for the control
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

    template <typename Control>
    requires std::derived_from<Control, GUIControl> Control *getControl(int id)
    {
        auto it = std::find_if(controls_.begin(), controls_.end(), [&](auto &control) {
            return control && control->id() == id;
        });

        if (it == controls_.end()) return nullptr;

        return (Control *)it->get();
    }

    /**
     * Creates a window, type parameterizing its layout, because we do not need to
     * worry about creating it ourselves.
     *
     * (We also need a way to pass parameters to future layouts, but a parameter
     * pack might be OK)
     */
    template <typename Layout>
    requires std::derived_from<Layout, BaseLayout> GUIWindow &createWindow(std::string name)
    {
        static int winid = 1;
        auto layoutptr   = std::make_unique<Layout>();
        render_info_.gm  = (void *)this;

        auto ptr       = std::make_unique<GUIWindow>(*layoutptr.get(), render_info_);
        GUIWindow *ret = ptr.get();
        ptr->initialize(winid);

        auto paintdata = painter_->drawWindow(*ret);

        windows_.push_back(
            WindowInfo{name, std::move(ptr), std::move(layoutptr), std::move(paintdata)});
        winid++;

        ret->onResize(width_, height_, 0, 0);
        return *ret;
    }

    /**
     * Create a layout, type parameterizing it, and make the GUI manager manage its lifetime
     *
     * Usually you would use this when creating layouts for boxes
     */
    template <typename Layout, typename... Args>
    requires std::derived_from<Layout, BaseLayout> BaseLayout &createLayout(Args &&...args)
    {
        auto ptr = std::unique_ptr<BaseLayout>(new Layout(args...));
        auto ret = ptr.get();

        layouts_.push_back(std::move(ptr));
        return (BaseLayout &)*ret;
    }

    GUIWindow *getWindow(std::string name);

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

    void moveWindowToTop(GUIWindow &);

    /**
     * Removes a window from the window list.
     *
     * We use a name reference instead of a pointer because the name might not exist anymore.
     *
     */
    void destroyWindow(std::string name);

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
    void pushEvent(FGUIEventCallback cb, GUIControl &control)
    {
        events_.push(EventInfo{cb, control});
    }

    GUIRenderer &getRenderer() { return *renderer_.get(); }

    void debugClear() { debugOut_ = ""; }

    /**
     * Write to a debug pane that is cleared on each frame.
     */
    void debugWrite(std::string v) { debugOut_ += v; }

private:
    std::string debugOut_;

    std::vector<std::unique_ptr<GUIControl>> controls_;
    std::vector<WindowInfo> windows_;
    std::vector<std::unique_ptr<BaseLayout>> layouts_;

    std::queue<std::pair<FGUIEventCallback, int>> callbacks_;

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

    int width_  = 1;
    int height_ = 1;

    /// Sort windows by zIndex
    /// The higher the zIndex, the closer to the first element it is.
    void sortWindows();

    void registerEvent(FGUIEventCallback handler, int control_id);
};

class GUIScriptRunner
{
public:
    GUIScriptRunner(GUIManager *manager);

    void load(std::string file);

    /**
     * An enumerator for telling the possible layout options for
     * a GUIBox, since we cannot decide template types at runtime.
     */
    enum SchemeLayout { FlexVertical, FlexHorizontal, Unknown };

    static SchemeLayout getLayoutFromScheme(s7_scheme* sc, s7_pointer layout);

    static std::optional<std::string> getWindowNameFromScript(s7_scheme* sc, s7_pointer window);
    static std::optional<std::string> getControlNameFromScript(s7_scheme* sc, s7_pointer control);

    /**
     * Create a control representation to be sent to the script
     *
     * If you already know the type, to speed things up, you can
     * inform it in the `type` variable
     */
    static s7_pointer createControlToScript(s7_scheme* sc, 
        std::string name, const GUIControl &control, std::string type = "");

    /**
     * Get a color value from its scheme counterpart, a scheme vector of values
     * this is documentation about vectors:
     *  <https://www.gnu.org/software/guile/manual/html_node/Vectors.html>
     */
    static std::array<double, 4> getColorFromScript(s7_scheme* sc, s7_pointer color);

    void registerPublicFunction(std::string name, std::function<s7_pointer(s7_scheme*,s7_pointer)> fun)
    {
        env_.registerPublicFunction(name, fun);
    }

    GUIWindow *openMainWindow();

private:
    familyline::logic::ScriptEnvironment env_;
};

}  // namespace familyline::graphics::gui

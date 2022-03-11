#include <config.h>

#include <client/graphical/gui/gui_manager.hpp>
#include <common/logger.hpp>
#include <optional>
#include <range/v3/all.hpp>

#include "s7.h"

using namespace familyline::graphics::gui;
using namespace familyline::logic;

/**
 * Creates an window
 *
 * We will return a list to the scheme script, in this format:
 * `(name type id)`
 *
 * This is sort of a "default type" for a control in scheme.
 *  - name is the window name
 *  - type is the control type, in this case, "window"
 *  - id is the window ID
 *
 * Those values are not fixed, and you should not depend on them.
 * You should treat this object as an opaque thing.
 */
s7_pointer current_manager_add_window(
    s7_scheme *sc, s7_pointer args)  // s7_pointer name, s7_pointer layout)
{
    auto name   = s7_car(args);
    auto layout = s7_cadr(args);

    auto &log = familyline::LoggerService::getLogger();
    auto gm   = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);

    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(sc, name);
    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "current-manager-add-window: incorrect type for 'name'");
        return s7_f(sc);
    }

    auto slayout = GUIScriptRunner::getLayoutFromScheme(sc, layout);

    auto id = 0;

    switch (slayout) {
        case GUIScriptRunner::SchemeLayout::FlexHorizontal: {
            GUIWindow &w = gm->createWindow<FlexLayout<true>>(*sname);
            id           = w.id();
            break;
        }
        case GUIScriptRunner::SchemeLayout::FlexVertical: {
            GUIWindow &w = gm->createWindow<FlexLayout<false>>(*sname);
            id           = w.id();
            break;
        }
        default:
            log->write(
                "gui-script-env", familyline::LogType::Warning, "unknown layout type for window {}",
                *sname);
            return s7_f(sc);
    }

    return s7_list(
        sc, 3, s7_make_string(sc, sname->c_str()), s7_make_string(sc, "window"),
        s7_make_integer(sc, id));
}

/**
 * From the window object, get the window name
 */
std::optional<std::string> GUIScriptRunner::getWindowNameFromScript(
    s7_scheme *sc, s7_pointer window)
{
    if (!s7_is_list(sc, window)) {
        return std::nullopt;
    }

    s7_pointer wname = s7_list_ref(sc, window, 0);
    return ScriptEnvironment::convertTypeFrom<std::string>(sc, wname);
}

/**
 * From the control object, get the control name
 */
std::optional<std::string> GUIScriptRunner::getControlNameFromScript(
    s7_scheme *sc, s7_pointer control)
{
    auto &log = familyline::LoggerService::getLogger();
    if (!s7_is_pair(control)) {
        log->write(
            "gui-script-env", familyline::LogType::Error, "invalid format for the control: {}",
            std::make_pair(sc, control));
        return std::nullopt;
    }

    return ScriptEnvironment::convertTypeFrom<std::string>(sc, s7_cdr(control));
}

std::string getControlTypeString(const GUIControl *c)
{
    if (dynamic_cast<const GUIBox *>(c)) {
        return "box";
    } else if (dynamic_cast<const GUILabel *>(c)) {
        return "label";
    } else if (dynamic_cast<const GUICheckbox *>(c)) {
        return "checkbox";
    } else if (dynamic_cast<const GUITextbox *>(c)) {
        return "textbox";
    } else if (dynamic_cast<const GUIButton *>(c)) {
        return "button";
    } else if (dynamic_cast<const GUIImageView *>(c)) {
        return "imageview";
    } else {
        return "unknown";
    }
}

/**
 * Create a control representation to be sent to the script
 */
s7_pointer GUIScriptRunner::createControlToScript(
    s7_scheme *sc, std::string name, const GUIControl &control, std::string type)
{
    if (type == "") {
        type = getControlTypeString(&control);
    }

    return s7_cons(sc, s7_make_keyword(sc, type.c_str()), s7_make_string(sc, name.c_str()));
}

/**
 * Adds an existing control to a window
 *
 * On scheme, you would use (window-add-control window control)
 */
s7_pointer window_add_control(s7_scheme *sc, s7_pointer args)
{
    auto window  = s7_car(args);
    auto control = s7_cadr(args);

    auto &log        = familyline::LoggerService::getLogger();
    auto gm          = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    auto winname     = GUIScriptRunner::getWindowNameFromScript(sc, window);
    auto controlname = GUIScriptRunner::getControlNameFromScript(sc, control);

    if (!winname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-add-control: incorrect type for 'window'");
        return s7_f(sc);
    }

    if (!controlname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-add-control: incorrect control object on window '{}'", *winname);
        return s7_f(sc);
    }

    GUIWindow *w = gm->getWindow(*winname);
    if (!window) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-add-control: window '{}' does not exist", *winname);
        return s7_f(sc);
    }

    GUIControl *c = gm->getControl<GUIControl>(*controlname);
    if (!c) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-add-control: control '{}' does not exist", *controlname);
        return s7_f(sc);
    }

    w->box().add(c);

    return s7_make_string(sc, controlname->c_str());
}

/**
 * Set an appearance element of a control.
 *
 * The attributes are defined as an association list, like this:
 *
 * ```scheme
 * (set-appearance-of (control-get "l1")
 *                   '((background . #(0.2 0 0 1))
 *                     (foreground . #(0 1 1 1))))
 *
 * ```
 * Here above, we set two appearance elements: background and foreground.
 */
s7_pointer set_appearance_of(s7_scheme *sc, s7_pointer args)
{
    auto control    = s7_car(args);
    auto attributes = s7_cadr(args);

    auto &log        = familyline::LoggerService::getLogger();
    auto controlname = GUIScriptRunner::getControlNameFromScript(sc, control);
    if (!controlname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "set-appeance-of: incorrect control object");
        return s7_f(sc);
    }

    auto gm       = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    GUIControl *c = gm->getControl<GUIControl>(*controlname);
    if (!c) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "set-appeance-of: control {} does not exist", *controlname);
        return s7_f(sc);
    }

    GUIAppearance a = c->appearance();

    while (!s7_is_null(sc, attributes)) {
        s7_pointer item = s7_car(attributes);

        s7_pointer key   = s7_car(item);
        s7_pointer value = s7_cdr(item);

        std::string skey = s7_symbol_name(key);

        if (skey == "background")
            a.background = GUIScriptRunner::getColorFromScript(sc, value);
        else if (skey == "foreground")
            a.foreground = GUIScriptRunner::getColorFromScript(sc, value);
        else if (skey == "font-size")
            a.fontsize = ScriptEnvironment::convertTypeFrom<size_t>(sc, value).value();
        else if (skey == "font")
            a.font = ScriptEnvironment::convertTypeFrom<std::string>(sc, value).value();
        else if (skey == "min-height")
            a.minHeight = ScriptEnvironment::convertTypeFrom<unsigned>(sc, value);
        else if (skey == "min-width")
            a.minWidth = ScriptEnvironment::convertTypeFrom<unsigned>(sc, value);
        else if (skey == "max-height")
            a.maxHeight = ScriptEnvironment::convertTypeFrom<unsigned>(sc, value);
        else if (skey == "max-width")
            a.maxWidth = ScriptEnvironment::convertTypeFrom<unsigned>(sc, value);

        attributes = s7_cdr(attributes);
    }

    c->setAppearance(a);
    return s7_t(sc);
}

/**
 * Add a control to a box
 *
 * In scheme, you would call as (box-add box control)
 */
s7_pointer box_add(s7_scheme *sc, s7_pointer args)
{
    auto box     = s7_car(args);
    auto control = s7_cadr(args);

    auto &log        = familyline::LoggerService::getLogger();
    auto boxname     = GUIScriptRunner::getControlNameFromScript(sc, box);
    auto controlname = GUIScriptRunner::getControlNameFromScript(sc, control);

    if (!controlname) {
        log->write(
            "gui-script-env", familyline::LogType::Error, "box-add: incorrect control object");
        return s7_f(sc);
    }
    if (!boxname) {
        log->write("gui-script-env", familyline::LogType::Error, "box-add: incorrect box object");
        return s7_f(sc);
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    GUIBox *gbox = gm->getControl<GUIBox>(*boxname);
    if (!gbox) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "box-add: box {} does not exist or is not a box", *boxname);
        return s7_f(sc);
    }
    GUIControl *gcontrol = gm->getControl<GUIControl>(*controlname);
    if (!gcontrol) {
        log->write(
            "gui-script-env", familyline::LogType::Error, "box-add: control {} does not exist",
            *controlname);
        return s7_f(sc);
    }

    gbox->add(gcontrol);
    return GUIScriptRunner::createControlToScript(sc, *controlname, *gbox, "box");
}

/**
 * Create a box
 * The form is (control-create-box name layout children)
 *  - name is the control name
 *  - layout is the layout definition, like (flex horizontal) or something like that
 *  - children is the children you want to add here
 */
s7_pointer control_create_box(s7_scheme *sc, s7_pointer args)
{
    auto name     = s7_car(args);
    auto layout   = s7_cadr(args);
    auto children = s7_caddr(args);

    auto &log = familyline::LoggerService::getLogger();

    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(sc, name);

    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-box: incorrect type for 'name'");
        return s7_f(sc);
    }

    auto gm                               = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    GUIScriptRunner::SchemeLayout slayout = GUIScriptRunner::getLayoutFromScheme(sc, layout);

    GUIBox *b = nullptr;
    switch (slayout) {
        case GUIScriptRunner::SchemeLayout::FlexHorizontal: {
            auto &layout = gm->createLayout<FlexLayout<true>>();
            b            = gm->createNamedControl<GUIBox>(*sname, layout);
            break;
        }
        case GUIScriptRunner::SchemeLayout::FlexVertical: {
            auto &layout = gm->createLayout<FlexLayout<false>>();
            b            = gm->createNamedControl<GUIBox>(*sname, layout);
            break;
        }
        default: return s7_f(sc);
    }

    if (!s7_is_list(sc, children)) {
        log->write(
            "gui-script-env", familyline::LogType::Warning,
            "control-create-box: box has no children?");
    }

    while (!s7_is_null(sc, children)) {
        s7_pointer child        = s7_car(children);
        std::string controlname = GUIScriptRunner::getControlNameFromScript(sc, child).value();
        b->add(gm->getControl<GUIControl>(controlname));
        children = s7_cdr(children);
    }

    return GUIScriptRunner::createControlToScript(sc, *sname, *b, "box");
}

/**
 * Creates a label
 *
 * (control-create-label name text)
 *
 * - name is the label name
 * - text is the label text
 */
s7_pointer control_create_label(s7_scheme *sc, s7_pointer args)
{
    auto name = s7_car(args);
    auto text = s7_cadr(args);

    auto &log  = familyline::LoggerService::getLogger();
    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(sc, name);
    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-label: incorrect type for 'name'");
        return s7_f(sc);
    }

    auto stext = ScriptEnvironment::convertTypeFrom<std::string>(sc, text);
    if (!stext) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-label: incorrect type for 'text'");
        return s7_f(sc);
    }

    auto gm    = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    auto label = gm->createNamedControl<GUILabel>(*sname, *stext);
    return GUIScriptRunner::createControlToScript(sc, *sname, *label, "label");
}

/**
 * Creates a textbox
 *
 * (control-create-textbox name text)
 *
 * - name is the textbox name
 * - text is the textbox text
 */
s7_pointer control_create_textbox(s7_scheme *sc, s7_pointer args)
{
    auto name = s7_car(args);
    auto text = s7_cadr(args);

    auto &log  = familyline::LoggerService::getLogger();
    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(sc, name);
    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-textbox: incorrect type for 'name': {}",
            std::make_pair(sc, name));
        return s7_f(sc);
    }

    auto stext = ScriptEnvironment::convertTypeFrom<std::string>(sc, text);
    if (!stext) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-textbox: incorrect type for 'text': {}",
            std::make_pair(sc, text));
        return s7_f(sc);
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    auto textbox = gm->createNamedControl<GUITextbox>(*sname, *stext);
    return GUIScriptRunner::createControlToScript(sc, *sname, *textbox, "textbox");
}

/**
 * Creates a checkbox
 *
 * (control-create-checkbox name active)
 *
 * - name: the checkbox name
 * - active: #t if the checkbox is checked, #f if it is not
 */
s7_pointer control_create_checkbox(s7_scheme *sc, s7_pointer args)
{
    auto name   = s7_car(args);
    auto active = s7_cadr(args);

    auto &log  = familyline::LoggerService::getLogger();
    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(sc, name);
    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-checkbox: incorrect type for 'name': {}",
            std::make_pair(sc, name));
        return s7_f(sc);
    }

    auto bactive = ScriptEnvironment::convertTypeFrom<bool>(sc, active);
    if (!bactive) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-checkbox: incorrect type for 'active': {}",
            std::make_pair(sc, active));
        return s7_f(sc);
    }

    auto gm       = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    auto checkbox = gm->createNamedControl<GUICheckbox>(*sname, *bactive);
    return GUIScriptRunner::createControlToScript(sc, *sname, *checkbox, "checkbox");
}

/**
 * Creates a button
 *
 * (control-create-button name text click_handler)
 *
 * - name is the button name
 * - text is the button text
 * - click_handler is a callable function that will run when you
 *   click the button
 *
 * - The handler must be defined as a function like this:
 *   (lambda (control))
 *   where `control` is the object representing the control
 *   that was clicked.
 */
s7_pointer control_create_button(s7_scheme *sc, s7_pointer args)
{
    auto name          = s7_car(args);
    auto text          = s7_cadr(args);
    auto click_handler = s7_caddr(args);

    auto &log  = familyline::LoggerService::getLogger();
    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(sc, name);
    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-button: incorrect type for 'name'");
        return s7_f(sc);
    }

    auto stext = ScriptEnvironment::convertTypeFrom<std::string>(sc, text);
    if (!stext) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-button: incorrect type for 'text'");
        return s7_f(sc);
    }

    auto gm = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);

    s7_gc_protect(sc, click_handler);
    auto button = gm->createNamedControl<GUIButton>(*sname, *stext, [=](GUIControl &c) {
        s7_call(
            sc, click_handler,
            s7_list(sc, 1, GUIScriptRunner::createControlToScript(sc, *sname, c, "button")));
    });

    return GUIScriptRunner::createControlToScript(sc, *sname, *button, "button");
}

/**
 * From a name, returns the control object
 * You call this from scheme as (control-get name)
 *
 * If the control does not exist, returns #f
 */
s7_pointer control_get(s7_scheme* sc, s7_pointer args)
{
    auto name = s7_car(args);
    
    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(sc, name);
    if (!sname) {
        return s7_f(sc);
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    auto control = gm->getControl<GUIControl>(*sname);
    return control ? GUIScriptRunner::createControlToScript(sc, *sname, *control) : s7_f(sc);
}

/**
 * Set some button attribute, excluding appareance ones
 *
 * You call this from scheme as (control-set-button control property value)
 *
 * The property is defined as a symbol (like 'text, or 'handler).
 */
s7_pointer control_set_button(s7_scheme* sc, s7_pointer args)
{
    auto control = s7_car(args);
    auto property = s7_cadr(args);
    auto value = s7_caddr(args);
    
    auto &log = familyline::LoggerService::getLogger();

    if (!s7_is_symbol(property)) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-set-button: wrong type for property");
        return s7_f(sc);
    }

    auto sproperty = std::string{s7_symbol_name(property)};
    auto controlname = GUIScriptRunner::getControlNameFromScript(sc, control);
    if (!controlname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-set-button: wrong type for control");
        return s7_f(sc);
    }

    auto gm     = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    auto button = gm->getControl<GUIButton>(*controlname);

    if (sproperty == "text") {
        auto svalue = ScriptEnvironment::convertTypeFrom<std::string>(sc, value);
        button->setText(*svalue);
        log->write(
            "gui-script-env", familyline::LogType::Info, "setting button property {} to {}",
            sproperty, *svalue);
    }

    return control;
}

/**
 * Get the value of a certain property of the control
 * You would call it like this: (control-get-textbox-property control property)
 *
 * The only property for a textbox is its text
 *
 * For unknown properties, return #f
 */
s7_pointer control_get_textbox_property(s7_scheme* sc, s7_pointer args)
{
    auto control = s7_car(args);
    auto property = s7_cadr(args);
    
    auto &log = familyline::LoggerService::getLogger();
    if (!s7_is_symbol(property)) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-set-button: wrong type for property");
        return s7_f(sc);
    }

    auto sproperty = std::string{s7_symbol_name(property)};

    auto controlname = GUIScriptRunner::getControlNameFromScript(sc, control);
    if (!controlname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-set-textbox: wrong type for control");
        return s7_f(sc);
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    auto textbox = gm->getControl<GUITextbox>(*controlname);

    if (sproperty == "text") {
        auto text = textbox->text();
        return s7_make_string(sc, text.c_str());
    }

    return s7_f(sc);
}

/**
 * Get the value of a certain property of the checkbox
 * You would call it like this: (control-get-checkbox-property control property)
 *
 * The only property for a textbox is its active property
 *
 * For unknown properties, return #f
 */
s7_pointer control_get_checkbox_property(s7_scheme* sc, s7_pointer args)
{
    auto &log = familyline::LoggerService::getLogger();
    auto control = s7_car(args);
    auto property = s7_cadr(args);
    
    if (!s7_is_symbol(property)) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-set-button: wrong type for property");
        return s7_f(sc);
    }

    auto sproperty = std::string{s7_symbol_name(property)};

    auto controlname = GUIScriptRunner::getControlNameFromScript(sc, control);
    if (!controlname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-set-checkbox: wrong type for control");
        return s7_f(sc);
    }

    auto gm       = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    auto checkbox = gm->getControl<GUICheckbox>(*controlname);

    if (sproperty == "active") {
        auto active = checkbox->checked();
        return s7_make_boolean(sc, active);
    }

    return s7_f(sc);
}

/**
 * Show the specified window
 *
 * You call it like this: (window-show window)
 */
s7_pointer window_show(s7_scheme* sc, s7_pointer args)
{
    auto window = s7_car(args);
    
    auto &log    = familyline::LoggerService::getLogger();
    auto winname = GUIScriptRunner::getWindowNameFromScript(sc, window);
    if (!winname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "show-window: incorrect type for 'window'");
        return s7_f(sc);
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    GUIWindow *w = gm->getWindow(*winname);
    if (!w) {
        log->write(
            "gui-script-env", familyline::LogType::Error, "show-window: window '%s' does not exist",
            *winname);
        return s7_f(sc);
    }

    gm->showWindow(*w);
    return s7_list(
        sc, 3, s7_make_string(sc, winname->c_str()), s7_make_string(sc, "window"),
        s7_make_integer(sc, w->id()));
}

/**
 * Move the specified window to the top of the window stack
 * You would call it like this: (window-move-to-top window)
 *
 * We accept two types of "window" here:
 *  - a window object
 *  - a window name
 */
s7_pointer window_move_to_top(s7_scheme* sc, s7_pointer args)
{
    auto window = s7_car(args);
    
    auto &log    = familyline::LoggerService::getLogger();
    auto winname = GUIScriptRunner::getWindowNameFromScript(sc, window);
    if (!winname) {
        winname = ScriptEnvironment::convertTypeFrom<std::string>(sc, window);
    }

    if (!winname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-destroy: incorrect type for 'window': {}",
            std::make_pair(sc, window));
        return s7_f(sc);
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    GUIWindow *w = gm->getWindow(*winname);
    if (!w) {
        log->write(
            "gui-script-env", familyline::LogType::Error, "show-window: window '%s' does not exist",
            *winname);
        return s7_f(sc);
    }
    gm->moveWindowToTop(*w);
    return s7_list(
        sc, 3, s7_make_string(sc, winname->c_str()), s7_make_string(sc, "window"),
        s7_make_integer(sc, w->id()));
}

/**
 * Destroy the specified window
 * You would call it like this: (window-destroy window)
 *
 * We accept two types of "window" here:
 *  - a window object
 *  - a window name
 */
s7_pointer window_destroy(s7_scheme* sc, s7_pointer args)
{
    auto window = s7_car(args);
    
    auto &log    = familyline::LoggerService::getLogger();
    auto winname = GUIScriptRunner::getWindowNameFromScript(sc, window);
    if (!winname) {
        winname = ScriptEnvironment::convertTypeFrom<std::string>(sc, window);
    }

    if (!winname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-destroy: incorrect type for 'window': {}",
            std::make_pair(sc, window));
        return s7_f(sc);
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>(sc);
    GUIWindow *w = gm->getWindow(*winname);
    if (!w) {
        log->write(
            "gui-script-env", familyline::LogType::Error, "show-window: window '%s' does not exist",
            *winname);
        return s7_f(sc);
    }
    gm->closeWindow(*w);
    gm->destroyWindow(*winname);

    return s7_list(
        sc, 3, s7_make_string(sc, winname->c_str()), s7_make_string(sc, "window"),
        s7_make_integer(sc, w->id()));
}

GUIScriptRunner::GUIScriptRunner(GUIManager *manager)
    : env_(familyline::logic::ScriptEnvironment(manager))
{
    env_.registerFunction("current-manager-add-window", 2, current_manager_add_window);
    env_.registerFunction("window-add-control", 2, window_add_control);
    env_.registerFunction("set-appearance-of", 2, set_appearance_of);
    env_.registerFunction("box-add", 2, box_add);

    env_.registerFunction("control-create-box", 3, control_create_box);
    env_.registerFunction("control-create-label", 2, control_create_label);
    env_.registerFunction("control-create-button", 3, control_create_button);
    env_.registerFunction("control-create-textbox", 2, control_create_textbox);
    env_.registerFunction("control-create-checkbox", 2, control_create_checkbox);

    env_.registerFunction("control-get-textbox-property", 2, control_get_textbox_property);
    env_.registerFunction("control-get-checkbox-property", 2, control_get_checkbox_property);

    env_.registerFunction("control-get", 1, control_get);

    env_.registerFunction("control-set-button", 3, control_set_button);

    env_.registerFunction("window-show", 1, window_show);
    env_.registerFunction("window-destroy", 1, window_destroy);
    env_.registerFunction("window-move-to-top", 1, window_move_to_top);

    this->load(SCRIPTS_DIR "gui/gui-prelude.scm");
}

GUIWindow *GUIScriptRunner::openMainWindow()
{
    auto &log      = familyline::LoggerService::getLogger();
    s7_pointer win = env_.evalFunction("on-main-menu-open", s7_list(env_.getContext(),
                                                                    1, s7_t(env_.getContext())));
    GUIManager *gm = env_.get_value<GUIManager *>();

//    fprintf(stderr, "<%s>", scm_to_locale_string(scm_object_to_string(win, s7_pointer_UNDEFINED)));
    auto winname = GUIScriptRunner::getWindowNameFromScript(env_.getContext(), win);
    if (!winname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "cannot find main window in the script, the type of {} is incorrect",
            std::make_pair(env_.getContext(), win));
        return nullptr;
    }

    GUIWindow *w = gm->getWindow(*winname);
    if (!w) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "cannot find main window in the script, the window {} does not exist", *winname);
        return nullptr;
    }

    return w;
}

/**
 * In Scheme, a layout is a cons
 * The car is the layout type, the cdr is a parameter
 */
GUIScriptRunner::SchemeLayout GUIScriptRunner::getLayoutFromScheme(s7_scheme* sc, s7_pointer layout)
{
    auto &log = LoggerService::getLogger();
    if (!s7_is_pair(layout)) {
        log->write("script-env", LogType::Error, "layout spec from scheme is not a cons list.");
        return Unknown;
    }

    s7_pointer lcar = s7_car(layout);
    s7_pointer lcdr = s7_cdr(layout);

    std::string lcarstr = "";
    std::string lcdrstr = "";

    if (s7_is_symbol(lcar)) {
        lcarstr = s7_symbol_name(lcar);
    }
    if (s7_is_symbol(lcdr)) {
        lcdrstr = s7_symbol_name(lcdr);
    }

    if (lcarstr == "flex") {
        if (lcdrstr == "vertical")
            return FlexVertical;
        else if (lcdrstr == "horizontal")
            return FlexHorizontal;
    }

    log->write(
        "script-env", LogType::Warning, "unknown layout spec coming from scheme: ('{} '{})",
        lcarstr, lcdrstr);

    return Unknown;
}

/**
 * Get a color value from its scheme counterpart, a scheme vector of values
 * this is documentation about vectors:
 *  <https://www.gnu.org/software/guile/manual/html_node/Vectors.html>
 */
std::array<double, 4> GUIScriptRunner::getColorFromScript(s7_scheme* sc, s7_pointer color)
{
    switch (s7_vector_length(color)) {
    case 4:
        return {
            s7_real(s7_vector_ref(sc, color, 0)),
            s7_real(s7_vector_ref(sc, color, 1)),
            s7_real(s7_vector_ref(sc, color, 2)),
            s7_real(s7_vector_ref(sc, color, 3))
        };
    case 3:
        return {
            s7_real(s7_vector_ref(sc, color, 0)),
            s7_real(s7_vector_ref(sc, color, 1)),
            s7_real(s7_vector_ref(sc, color, 2)),
            1.0
        };
    default:
        return {0,0,0,0};
    }
}

void GUIScriptRunner::load(std::string file) { env_.runScript(file); }

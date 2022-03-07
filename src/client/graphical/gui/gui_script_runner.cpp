#include <client/graphical/gui/gui_manager.hpp>
#include <common/logger.hpp>
#include <optional>
#include <range/v3/all.hpp>

#include "libguile/strings.h"

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
SCM current_manager_add_window(SCM name, SCM layout)
{
    auto &log = familyline::LoggerService::getLogger();
    auto gm   = ScriptEnvironment::getGlobalEnv<GUIManager *>();

    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(name);
    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "current-manager-add-window: incorrect type for 'name'");
        return SCM_BOOL_F;
    }

    auto slayout = GUIScriptRunner::getLayoutFromScheme(layout);

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
            return SCM_BOOL_F;
    }

    return scm_list_3(
        scm_from_locale_string(sname->c_str()), scm_from_locale_string("window"),
        scm_from_signed_integer(id));
}

/**
 * From the window object, get the window name
 */
std::optional<std::string> GUIScriptRunner::getWindowNameFromScript(SCM window)
{
    if (scm_list_p(window) == SCM_BOOL_F) {
        return std::nullopt;
    }    

    SCM wname = scm_list_ref(window, scm_from_uint32(0));
    return ScriptEnvironment::convertTypeFrom<std::string>(wname);
}

/**
 * From the control object, get the control name
 */
std::optional<std::string> GUIScriptRunner::getControlNameFromScript(SCM control)
{
    return ScriptEnvironment::convertTypeFrom<std::string>(scm_cdr(control));
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
SCM GUIScriptRunner::createControlToScript(
    std::string name, const GUIControl &control, std::string type)
{
    if (type == "") {
        type = getControlTypeString(&control);
    }

    return scm_cons(scm_from_locale_keyword(type.c_str()), scm_from_locale_string(name.c_str()));
}

/**
 * Adds an existing control to a window
 */
SCM window_add_control(SCM window, SCM control)
{
    auto &log        = familyline::LoggerService::getLogger();
    auto gm          = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    auto winname     = GUIScriptRunner::getWindowNameFromScript(window);
    auto controlname = GUIScriptRunner::getControlNameFromScript(control);

    if (!winname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-add-control: incorrect type for 'window'");
        return SCM_BOOL_F;
    }

    if (!controlname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-add-control: incorrect control object on window '{}'", *winname);
        return SCM_BOOL_F;
    }

    GUIWindow *w = gm->getWindow(*winname);
    if (!window) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-add-control: window '%s' does not exist", *winname);
        return SCM_BOOL_F;
    }

    GUIControl *c = gm->getControl<GUIControl>(*controlname);
    if (!c) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-add-control: control '%s' does not exist", *controlname);
        return SCM_BOOL_F;
    }

    w->box().add(c);

    return scm_from_locale_string(controlname->c_str());
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
SCM set_appearance_of(SCM control, SCM attributes)
{
    auto &log        = familyline::LoggerService::getLogger();
    auto controlname = GUIScriptRunner::getControlNameFromScript(control);
    if (!controlname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "set-appeance-of: incorrect control object");
        return SCM_BOOL_F;
    }

    auto gm       = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    GUIControl *c = gm->getControl<GUIControl>(*controlname);
    if (!c) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "set-appeance-of: control {} does not exist", *controlname);
        return SCM_BOOL_F;
    }

    GUIAppearance a = c->appearance();

    while (!scm_to_bool(scm_null_p(attributes))) {
        SCM item = scm_car(attributes);

        SCM key   = scm_car(item);
        SCM value = scm_cdr(item);

        std::string skey = scm_to_locale_string(scm_symbol_to_string(key));

        if (skey == "background")
            a.background = GUIScriptRunner::getColorFromScript(value);
        else if (skey == "foreground")
            a.foreground = GUIScriptRunner::getColorFromScript(value);
        else if (skey == "font-size")
            a.fontsize = ScriptEnvironment::convertTypeFrom<size_t>(value).value();
        else if (skey == "font")
            a.font = ScriptEnvironment::convertTypeFrom<std::string>(value).value();
        else if (skey == "min-height")
            a.minHeight = ScriptEnvironment::convertTypeFrom<unsigned>(value);
        else if (skey == "min-width")
            a.minWidth = ScriptEnvironment::convertTypeFrom<unsigned>(value);
        else if (skey == "max-height")
            a.maxHeight = ScriptEnvironment::convertTypeFrom<unsigned>(value);
        else if (skey == "max-width")
            a.maxWidth = ScriptEnvironment::convertTypeFrom<unsigned>(value);

        attributes = scm_cdr(attributes);
    }

    c->setAppearance(a);
    return SCM_BOOL_T;
}

/**
 * Add a control to a box
 */
SCM box_add(SCM box, SCM control)
{
    auto &log        = familyline::LoggerService::getLogger();
    auto boxname     = GUIScriptRunner::getControlNameFromScript(box);
    auto controlname = GUIScriptRunner::getControlNameFromScript(control);

    if (!controlname) {
        log->write(
            "gui-script-env", familyline::LogType::Error, "box-add: incorrect control object");
        return SCM_BOOL_F;
    }
    if (!boxname) {
        log->write("gui-script-env", familyline::LogType::Error, "box-add: incorrect box object");
        return SCM_BOOL_F;
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    GUIBox *gbox = gm->getControl<GUIBox>(*boxname);
    if (!gbox) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "box-add: box {} does not exist or is not a box", *boxname);
        return SCM_BOOL_F;
    }
    GUIControl *gcontrol = gm->getControl<GUIControl>(*controlname);
    if (!gcontrol) {
        log->write(
            "gui-script-env", familyline::LogType::Error, "box-add: control {} does not exist",
            *controlname);
        return SCM_BOOL_F;
    }

    gbox->add(gcontrol);
    return GUIScriptRunner::createControlToScript(*controlname, *gbox, "box");
}

/**
 * Create a box
 * The form is (control-create-box name layout children)
 *  - name is the control name
 *  - layout is the layout definition, like (flex horizontal) or something like that
 *  - children is the children you want to add here
 */
SCM control_create_box(SCM name, SCM layout, SCM children)
{
    auto &log = familyline::LoggerService::getLogger();

    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(name);

    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-box: incorrect type for 'name'");
        return SCM_BOOL_F;
    }

    auto gm                               = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    GUIScriptRunner::SchemeLayout slayout = GUIScriptRunner::getLayoutFromScheme(layout);

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
        default: return SCM_BOOL_F;
    }

    if (!scm_list_p(children)) {
        log->write(
            "gui-script-env", familyline::LogType::Warning,
            "control-create-box: box has no children?");
    }

    while (!scm_to_bool(scm_null_p(children))) {
        SCM child               = scm_car(children);
        std::string controlname = GUIScriptRunner::getControlNameFromScript(child).value();
        printf("CHILD: %s\n", controlname.c_str());
        b->add(gm->getControl<GUIControl>(controlname));
        children = scm_cdr(children);
    }

    return GUIScriptRunner::createControlToScript(*sname, *b, "box");
}

/**
 * Creates a label
 *
 * (control-create-label name text)
 *
 * - name is the label name
 * - text is the label text
 */
SCM control_create_label(SCM name, SCM text)
{
    auto &log  = familyline::LoggerService::getLogger();
    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(name);
    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-label: incorrect type for 'name'");
        return SCM_BOOL_F;
    }

    auto stext = ScriptEnvironment::convertTypeFrom<std::string>(text);
    if (!stext) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-label: incorrect type for 'text'");
        return SCM_BOOL_F;
    }

    auto gm    = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    auto label = gm->createNamedControl<GUILabel>(*sname, *stext);
    return GUIScriptRunner::createControlToScript(*sname, *label, "label");
}

/**
 * Creates a textbox
 *
 * (control-create-textbox name text)
 *
 * - name is the textbox name
 * - text is the textbox text
 */
SCM control_create_textbox(SCM name, SCM text)
{
    auto &log  = familyline::LoggerService::getLogger();
    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(name);
    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-textbox: incorrect type for 'name': {}", name);
        return SCM_BOOL_F;
    }

    auto stext = ScriptEnvironment::convertTypeFrom<std::string>(text);
    if (!stext) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-textbox: incorrect type for 'text': {}", text);
        return SCM_BOOL_F;
    }

    auto gm    = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    auto textbox = gm->createNamedControl<GUITextbox>(*sname, *stext);
    return GUIScriptRunner::createControlToScript(*sname, *textbox, "textbox");
}

/**
 * Creates a checkbox
 *
 * (control-create-checkbox name active)
 *
 * - name: the checkbox name
 * - active: #t if the checkbox is checked, #f if it is not
 */
SCM control_create_checkbox(SCM name, SCM active)
{
    auto &log  = familyline::LoggerService::getLogger();
    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(name);
    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-checkbox: incorrect type for 'name': {}", name);
        return SCM_BOOL_F;
    }

    auto bactive = ScriptEnvironment::convertTypeFrom<bool>(active);
    if (!bactive) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-checkbox: incorrect type for 'active': {}", active);
        return SCM_BOOL_F;
    }

    auto gm    = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    auto checkbox = gm->createNamedControl<GUICheckbox>(*sname, *bactive);
    return GUIScriptRunner::createControlToScript(*sname, *checkbox, "checkbox");
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
SCM control_create_button(SCM name, SCM text, SCM click_handler)
{
    auto &log  = familyline::LoggerService::getLogger();
    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(name);
    if (!sname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-button: incorrect type for 'name'");
        return SCM_BOOL_F;
    }

    auto stext = ScriptEnvironment::convertTypeFrom<std::string>(text);
    if (!stext) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-create-button: incorrect type for 'text'");
        return SCM_BOOL_F;
    }

    auto gm = ScriptEnvironment::getGlobalEnv<GUIManager *>();

    scm_gc_protect_object(click_handler);
    auto button = gm->createNamedControl<GUIButton>(*sname, *stext, [=](GUIControl &c) {
        scm_call_1(click_handler, GUIScriptRunner::createControlToScript(*sname, c, "button"));
    });

    return GUIScriptRunner::createControlToScript(*sname, *button, "button");
}

/**
 * From a name, returns the control object
 *
 * If the control does not exist, returns #f
 */
SCM control_get(SCM name)
{
    auto sname = ScriptEnvironment::convertTypeFrom<std::string>(name);
    if (!sname) {
        return SCM_BOOL_F;
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    auto control = gm->getControl<GUIControl>(*sname);
    return control ? GUIScriptRunner::createControlToScript(*sname, *control) : SCM_BOOL_F;
}

/**
 * Set some button attribute, excluding appareance ones
 */
SCM control_set_button(SCM control, SCM property, SCM value)
{
    auto &log = familyline::LoggerService::getLogger();
    auto sproperty =
        ScriptEnvironment::convertTypeFrom<std::string>(scm_symbol_to_string(property));

    if (!sproperty) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-set-button: wrong type for property");
        return SCM_BOOL_F;
    }

    auto controlname = GUIScriptRunner::getControlNameFromScript(control);
    if (!controlname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "control-set-button: wrong type for control");
        return SCM_BOOL_F;
    }

    auto gm     = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    auto button = gm->getControl<GUIButton>(*controlname);

    if (*sproperty == "text") {
        auto svalue = ScriptEnvironment::convertTypeFrom<std::string>(value);
        button->setText(*svalue);
        log->write(
            "gui-script-env", familyline::LogType::Info, "setting button property {} to {}",
            *sproperty, *svalue);
    }

    return control;
}

/**
 * Show the specified window
 */
SCM window_show(SCM window)
{
    auto &log    = familyline::LoggerService::getLogger();
    auto winname = GUIScriptRunner::getWindowNameFromScript(window);
    if (!winname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "show-window: incorrect type for 'window'");
        return SCM_BOOL_F;
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    GUIWindow *w = gm->getWindow(*winname);
    if (!w) {
        log->write(
            "gui-script-env", familyline::LogType::Error, "show-window: window '%s' does not exist",
            *winname);
        return SCM_BOOL_F;
    }

    gm->showWindow(*w);
    return scm_list_3(
        scm_from_locale_string(winname->c_str()), scm_from_locale_string("window"),
        scm_from_signed_integer(w->id()));
}

/**
 * Move the specified window to the top of the window stack
 *
 * We accept two types of "window" here:
 *  - a window object
 *  - a window name
 */
SCM window_move_to_top(SCM window)
{
    auto &log    = familyline::LoggerService::getLogger();
    auto winname = GUIScriptRunner::getWindowNameFromScript(window);
    if (!winname) {
        winname = ScriptEnvironment::convertTypeFrom<std::string>(window);
    }

    if (!winname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-destroy: incorrect type for 'window': {}", window);
        return SCM_BOOL_F;
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    GUIWindow *w = gm->getWindow(*winname);
    if (!w) {
        log->write(
            "gui-script-env", familyline::LogType::Error, "show-window: window '%s' does not exist",
            *winname);
        return SCM_BOOL_F;
    }
    gm->moveWindowToTop(*w);

    return scm_list_3(
        scm_from_locale_string(winname->c_str()), scm_from_locale_string("window"),
        scm_from_signed_integer(w->id()));
}

/**
 * Show the specified window
 *
 * We accept two types of "window" here:
 *  - a window object
 *  - a window name
 */
SCM window_destroy(SCM window)
{
    auto &log    = familyline::LoggerService::getLogger();
    auto winname = GUIScriptRunner::getWindowNameFromScript(window);
    if (!winname) {
        winname = ScriptEnvironment::convertTypeFrom<std::string>(window);
    }

    if (!winname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "window-destroy: incorrect type for 'window': {}", window);
        return SCM_BOOL_F;
    }

    auto gm      = ScriptEnvironment::getGlobalEnv<GUIManager *>();
    GUIWindow *w = gm->getWindow(*winname);
    if (!w) {
        log->write(
            "gui-script-env", familyline::LogType::Error, "show-window: window '%s' does not exist",
            *winname);
        return SCM_BOOL_F;
    }
    gm->closeWindow(*w);
    gm->destroyWindow(*winname);

    return scm_list_3(
        scm_from_locale_string(winname->c_str()), scm_from_locale_string("window"),
        scm_from_signed_integer(w->id()));
}

GUIScriptRunner::GUIScriptRunner(GUIManager *manager)
    : env_(familyline::logic::ScriptEnvironment(manager))
{
    env_.registerFunction("current-manager-add-window", current_manager_add_window);
    env_.registerFunction("window-add-control", window_add_control);
    env_.registerFunction("set-appearance-of", set_appearance_of);
    env_.registerFunction("box-add", box_add);

    env_.registerFunction("control-create-box", control_create_box);
    env_.registerFunction("control-create-label", control_create_label);
    env_.registerFunction("control-create-button", control_create_button);
    env_.registerFunction("control-create-textbox", control_create_textbox);
    env_.registerFunction("control-create-checkbox", control_create_checkbox);

    env_.registerFunction("control-get", control_get);

    env_.registerFunction("control-set-button", control_set_button);

    env_.registerFunction("window-show", window_show);
    env_.registerFunction("window-destroy", window_destroy);
    env_.registerFunction("window-move-to-top", window_move_to_top);
}

GUIWindow *GUIScriptRunner::openMainWindow()
{
    auto &log      = familyline::LoggerService::getLogger();
    SCM win        = env_.evalFunction("on-main-menu-open", SCM_BOOL_F);
    GUIManager *gm = env_.get_value<GUIManager *>();

    fprintf(stderr, "<%s>", scm_to_locale_string(scm_object_to_string(win, SCM_UNDEFINED)));
    auto winname = GUIScriptRunner::getWindowNameFromScript(win);
    if (!winname) {
        log->write(
            "gui-script-env", familyline::LogType::Error,
            "cannot find main window in the script, the type of {} is incorrect", win);
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
GUIScriptRunner::SchemeLayout GUIScriptRunner::getLayoutFromScheme(SCM layout)
{
    auto &log = LoggerService::getLogger();
    if (!scm_pair_p(layout)) {
        log->write("script-env", LogType::Error, "layout spec from scheme is not a cons list.");
        return Unknown;
    }

    SCM lcar = scm_car(layout);
    SCM lcdr = scm_cdr(layout);

    std::string lcarstr = "";
    std::string lcdrstr = "";

    if (scm_symbol_p(lcar)) {
        lcarstr = scm_to_locale_string(scm_symbol_to_string(lcar));
    }
    if (scm_symbol_p(lcdr)) {
        lcdrstr = scm_to_locale_string(scm_symbol_to_string(lcdr));
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
std::array<double, 4> GUIScriptRunner::getColorFromScript(SCM color)
{
    return {
        scm_to_double(SCM_SIMPLE_VECTOR_REF(color, 0)),
        scm_to_double(SCM_SIMPLE_VECTOR_REF(color, 1)),
        scm_to_double(SCM_SIMPLE_VECTOR_REF(color, 2)),
        scm_to_double(SCM_SIMPLE_VECTOR_REF(color, 3))};
}

void GUIScriptRunner::load(std::string file) { env_.runScript(file); }

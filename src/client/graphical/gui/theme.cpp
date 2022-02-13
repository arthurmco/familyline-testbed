#include <fmt/core.h>
#include <yaml-cpp/node/parse.h>

#include <client/graphical/gui/gui.hpp>
#include <client/graphical/gui/theme.hpp>
#include <common/logger.hpp>

using namespace familyline::graphics::gui;

void GUITheme::loadFile(std::string path)
{
    auto &log = LoggerService::getLogger();
    try {
        root_node_ = YAML::LoadFile(path);

        log->write("gui-theme", LogType::Info, "loading theme from '{}'", path);
        log->write("gui-theme", LogType::Debug, "\tname: {}", root_node_["name"].as<std::string>());
        log->write(
            "gui-theme", LogType::Debug, "\tdescription: {}",
            root_node_["description"].as<std::string>());

        file_loaded_ = true;
    } catch (YAML::BadFile &b) {
        log->write("gui-theme", LogType::Warning, "could not load theme from '{}'", path);
        log->write(
            "gui-theme", LogType::Warning, "using the defaults (they might look ugly for you)");
        file_loaded_ = false;
    }
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
        if (cvalue.size() > 7) {
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

    /*
    if (appearance["font_weight"])
        gapp.weight = appearance["font_weight"].as();
    */

    return std::make_optional(gapp);
}

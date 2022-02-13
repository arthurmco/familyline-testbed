#pragma once

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>

#include <client/graphical/gui/gui_control.hpp>
#include <optional>
#include <string_view>

namespace familyline::graphics::gui
{
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

}  // namespace familyline::graphics::gui

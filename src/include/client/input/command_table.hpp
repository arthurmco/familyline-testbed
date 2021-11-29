#pragma once

/**
 * Mapping between keys and in-game user commands
 *
 * We read configuration information, parse them, and save them
 *
 * (C) 2021 Arthur Mendes
 */

#include <map>
#include <string>
#include <tuple>
#include <optional>
#include <vector>

#include <client/input/input_actions.hpp>

namespace familyline::input
{
    enum class PlayerCommandType { CameraMove, CameraRotate, CameraZoom,
        DebugCreateEntity,
        DebugDestroyEntity,
        DebugShowBoundingBox};

/**
 * The player command itself
 *
 * The first element of the tuple is the command, the second
 * is an optional parameter.
 */
using PlayerCommand = std::tuple<PlayerCommandType, std::string>;

/**
 * The key/input to command mapping, both strings.
 *
 * The key is a string representation of a key, or a mouse+key combo.
 * It follows Emacs conventions (because I use Emacs), so:
 *  - C- is ctrl
 *  - M- is alt
 *  - s- is the windows key
 *  - <up>, <down>, <left> and <right> are the arrow keys
 *  - the key letter, in lowercase, is the key name
 *
 * Mouse movements are described a little bit differently:
 *  - mouse-<button>-scroll is the scroll
 *  - mouse-<button>-click is the click.
 *  - mouse-<button>-double-click is the click.
 *
 * This way, stringfying the key, it is easier for some user to edit
 * than to write the keycode.
 *
 * The command mapping is the command name plus its parameter,
 * separated by a comma.
 */
using KeyConfiguration = std::pair<std::string, std::string>;

/**
 * This maps keys into user commands
 *
 * User commands are things that the player can do, more abstract than
 * launching an action and moving the camera, although it includes those
 * things too
 */
class CommandTable
{
public:
    /**
     * Converts an action into a possible command
     *
     * Not all actions are mapped to commands. In this case, we return a std::nullopt
     */
    std::optional<PlayerCommand> actionToCommand(const HumanInputAction& action) const;

    void loadConfiguration(std::vector<KeyConfiguration> keys);

    size_t size() const { return command_map_.size(); }

private:
    std::vector<std::pair<HumanInputType, PlayerCommand>> command_map_;
};

}  // namespace familyline::input

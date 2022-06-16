#include <client/input/command_table.hpp>

#include <common/logger.hpp>

using namespace familyline::input;

std::optional<HumanInputType> parseInputKey(std::string keydesc)
{
    bool isCtrl = false, isAlt = false, isShift = false;
    bool key_already = false, mouse_already = false;
    SDL_Keycode key;
    uint8_t button;

    static std::map<std::string, SDL_Keycode> keymap = {
        {"SPC", SDLK_SPACE},
        {"<backspace>", SDLK_BACKSPACE},
        {"<return>", SDLK_RETURN},
        {"<up>", SDLK_UP},
        {"<down>", SDLK_DOWN},
        {"<left>", SDLK_LEFT},
        {"<right>", SDLK_RIGHT},
        {"<backspace>", SDLK_BACKSPACE},
        {",", SDLK_COMMA},
        {"+", SDLK_PLUS},
        {"-", SDLK_MINUS},
        {"=", SDLK_EQUALS},
        {".", SDLK_PERIOD},
        {"/", SDLK_SLASH},
        {"*", SDLK_ASTERISK},

#include "keyset_gen.h"

        {"<kp-add>", SDLK_KP_PLUS},
        {"<kp-subtract>", SDLK_KP_MINUS},
        {"<kp-decimal>", SDLK_KP_PERIOD},
        {"<kp-multiply>", SDLK_KP_MULTIPLY},
        {"<kp-divide>", SDLK_KP_DIVIDE},
    };

    static std::map<std::string, uint8_t> buttonmap = {
        {"<mouse-1>", SDL_BUTTON_LEFT},   {"<mouse-2>", SDL_BUTTON_RIGHT},
        {"<mouse-3>", SDL_BUTTON_MIDDLE}, {"<mouse-4>", SDL_BUTTON_X1},
        {"<mouse-5>", SDL_BUTTON_X2},
    };

    for (auto idx = 0; idx < keydesc.size() && (!key_already && !mouse_already);) {
        auto str_remain = keydesc.substr(idx);

        if (str_remain.starts_with("C-")) {
            isCtrl = true;
            idx += 2;
            continue;
        }

        if (str_remain.starts_with("M-")) {
            isAlt = true;
            idx += 2;
            continue;
        }

        if (str_remain.starts_with("S-")) {
            isShift = true;
            idx += 2;
            continue;
        }

        auto spaceidx = str_remain.find_first_of(' ');
        auto keyname  = str_remain.substr(0, spaceidx);

        if (auto keyit = keymap.find(keyname); keyit != keymap.end()) {
            idx = (spaceidx == std::string::npos) ? idx = keydesc.size() : idx + spaceidx + 1;
            key = keyit->second;
            key_already = true;
            continue;
        }

        if (auto mouseit = buttonmap.find(keyname); mouseit != buttonmap.end()) {
            idx    = (spaceidx == std::string::npos) ? idx = keydesc.size() : idx + spaceidx + 1;
            button = mouseit->second;
            mouse_already = true;
            continue;
        }

        idx++;
    }

    if (!key_already && !mouse_already) return std::nullopt;

    int mods = (SDL_Keymod) int(isCtrl ? KMOD_CTRL : KMOD_NONE) |
               int(isAlt ? KMOD_ALT : KMOD_NONE) | int(isShift ? KMOD_SHIFT : KMOD_NONE);

    if (key_already) {
        return std::make_optional(KeyAction{
            .keycode    = key,
            .keyname    = nullptr,
            .isPressed  = true,
            .isRepeated = false,
            .modifiers  = (uint16_t)mods});
    }

    if (mouse_already) {
        return std::make_optional(ClickAction{
            .screenX      = 0,
            .screenY      = 0,
            .buttonCode   = button,
            .clickCount   = 1,
            .isPressed    = true,
            .keyModifiers = ((uint16_t)mods)});
    }

    return std::nullopt;
}

std::string trim(std::string value)
{
    auto lowerbound = value.find_first_not_of(" \t");
    auto upperbound = value.find_last_not_of(" \t");

    return value.substr(lowerbound, upperbound == std::string::npos ? upperbound : upperbound + 1);
}

std::optional<PlayerCommand> parseInputCommand(std::string command)
{
    auto commapos          = command.find_first_of(',');
    std::string strcommand = trim(command.substr(0, commapos));
    std::string strparam =
        (commapos == std::string::npos) ? "" : trim(command.substr(commapos + 1));

    static std::map<std::string, PlayerCommandType> command_map = {
        {"CameraMove", PlayerCommandType::CameraMove},
        {"CameraZoom", PlayerCommandType::CameraZoom},
        {"CameraRotate", PlayerCommandType::CameraRotate},
        {"DebugCreateEntity", PlayerCommandType::DebugCreateEntity},
        {"DebugDestroyEntity", PlayerCommandType::DebugDestroyEntity},
        {"DebugShowBoundingBox", PlayerCommandType::DebugShowBoundingBox}};

    if (command_map.contains(strcommand))
        return std::make_optional(std::make_tuple(command_map[strcommand], strparam));
    else
        return std::nullopt;
}

void CommandTable::loadConfiguration(std::vector<KeyConfiguration> keys)
{
    auto& log = LoggerService::getLogger();

    for (auto& [keydesc, command] : keys) {
        auto inputkey     = parseInputKey(keydesc);
        auto inputcommand = parseInputCommand(command);

        if (!inputkey) {
            log->write("command-table", LogType::Error,
                       "invalid key '{}' for command '{}'", keydesc, command);
            continue;
        }
        if (!inputcommand) {
            log->write("command-table", LogType::Error,
                       "invalid command '{}' for key '{}'", command, keydesc);
            continue;
        }

        log->write("command-table", LogType::Debug, "loaded key {} for command {}", keydesc, command);
        command_map_.emplace_back(*inputkey, *inputcommand);
    }
}

/**
 * Converts an action into a possible command
 *
 * Not all actions are mapped to commands. In this case, we return a std::nullopt
 */
std::optional<PlayerCommand> CommandTable::actionToCommand(const HumanInputAction& action) const
{
    auto& log = LoggerService::getLogger();

    /// We ignore those keys because they do not alter the meaning of the action.
    uint16_t ignoredKeys = KMOD_GUI | KMOD_CAPS | KMOD_NUM;
    
    auto it = std::find_if(command_map_.begin(), command_map_.end(), [&](auto& command) {
        auto& [inputaction, _inputcommand] = command;
        if (auto ikey = std::get_if<KeyAction>(&inputaction),
            akey      = std::get_if<KeyAction>(&action.type);
            ikey && akey) {
            return (
                ikey->keycode == akey->keycode &&
                ikey->isRepeated == akey->isRepeated &&
                ikey->modifiers == (akey->modifiers & ~ignoredKeys));
        }
        if (auto iclick = std::get_if<ClickAction>(&inputaction),
            aclick      = std::get_if<ClickAction>(&action.type);
            iclick && aclick) {
            return (
                iclick->buttonCode == aclick->buttonCode &&
                iclick->keyModifiers == aclick->keyModifiers);
        }

        return false;
    });

    if (it == command_map_.end()) {
        return std::nullopt;
    }

    return std::make_optional(it->second);
}

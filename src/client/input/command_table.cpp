#include <client/input/command_table.hpp>

using namespace familyline::input;


std::optional<HumanInputType> parseInputKey(std::string keydesc)
{
    bool isCtrl = false, isAlt = false, isShift = false, key_already = false;
    SDL_Keycode key;

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

    for (auto idx = 0; idx < keydesc.size() && !key_already;) {
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
        auto keyit    = keymap.find(keyname);

        if (keyit == keymap.end()) {
            idx = (spaceidx == std::string::npos) ? idx = keydesc.size() : spaceidx + 1;
            continue;
        }

        key         = keyit->second;
        key_already = true;
    }

    if (!key_already) return std::nullopt;

    int mods = (SDL_Keymod) int(isCtrl ? KMOD_CTRL : KMOD_NONE) |
               int(isAlt ? KMOD_ALT : KMOD_NONE) | int(isShift ? KMOD_SHIFT : KMOD_NONE);

    return std::make_optional(KeyAction{
        .keycode    = key,
        .keyname    = nullptr,
        .isPressed  = true,
        .isRepeated = false,
        .modifiers  = (uint16_t)mods});
}

std::string trim(std::string value) {
    auto lowerbound = value.find_first_not_of(" \t");
    auto upperbound = value.find_last_not_of(" \t");

    return value.substr(
        lowerbound,
        upperbound == std::string::npos ? upperbound : upperbound+1);
}


std::optional<PlayerCommand> parseInputCommand(std::string command) {
    auto commapos = command.find_first_of(',');
    std::string strcommand = trim(command.substr(0, commapos));
    std::string strparam = (commapos == std::string::npos) ? "" :
        trim(command.substr(commapos+1));

    if (strcommand == "CameraMove") {
        return std::make_optional(
            std::make_tuple(PlayerCommandType::CameraMove, strparam));
        
    } else if (strcommand == "CameraZoom") {
        return std::make_optional(
            std::make_tuple(PlayerCommandType::CameraZoom, strparam));        
    }

    return std::nullopt;
}

void CommandTable::loadConfiguration(std::vector<KeyConfiguration> keys)
{
    for (auto& [keydesc, command] : keys) {
        auto inputkey     = parseInputKey(keydesc);
        auto inputcommand = parseInputCommand(command);

        if (!inputkey) {
            fprintf(stderr, "invalid key: %s for command %s\n", keydesc.c_str(), command.c_str());
            continue;
        }
        if (!inputcommand) {
            fprintf(stderr, "invalid command: %s for key %s\n", command.c_str(), keydesc.c_str());
            continue;
        }

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
    auto it = std::find_if(command_map_.begin(), command_map_.end(), [&](auto& command) {
        auto& [inputaction, _inputcommand] = command;
        if (auto ikey = std::get_if<KeyAction>(&inputaction),
            akey = std::get_if<KeyAction>(&action.type);
            ikey && akey) {

            return (ikey->keycode == akey->keycode &&
                    ikey->isPressed == akey->isPressed &&
                    ikey->isRepeated == akey->isRepeated &&
                    ikey->modifiers == akey->modifiers);
            
        }

        return false;
    });

    if (it == command_map_.end()) return std::nullopt;

    return std::make_optional(it->second);
}

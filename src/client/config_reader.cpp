#include <cerrno>
#include <client/config_reader.hpp>
#include <string>
#include <fmt/format.h>

#include <yaml-cpp/yaml.h>
#include <common/logger.hpp>

using namespace familyline;

std::vector<std::string> familyline::get_config_valid_paths()
{
    std::string file_settings;

#if __has_include(<windows.h>) && __has_include(<winbase.h>)
    char* appdata = getenv("LOCALAPPDATA");

    if (appdata)
        file_settings = fmt::format("{}\\Familyline\\settings.yaml", appdata);
    else
        file_settings = "settings.yaml";

    return {
        "settings.yaml",
        file_settings,
    };

#else

    if (char* configdir = getenv("XDG_CONFIG_HOME"); configdir) {
        file_settings = fmt::format("{}/familyline/settings.yaml", configdir);
    } else if (char* home = getenv("HOME"); home) {
        file_settings = fmt::format("{}/.config/familyline/settings.yaml", home);
    } else if (char* logname = getenv("LOGNAME"); logname) {
        file_settings = fmt::format("/home/{}/.config/familyline/settings.yaml", logname);
    } else {
        file_settings = "/root/.config/familyline/settings.yaml";
    }

    return {"settings.yaml", "/etc/familyline/settings.yaml", file_settings};

#endif

    return {};
}

std::string replace_string(std::string str, std::string from, std::string to, size_t limit = 1)
{
    auto cidx = 0;
    auto fidx = str.find(from, cidx);

    auto i = 0;
    while (fidx != std::string::npos) {
        str.replace(fidx, from.size(), to);

        i++;
        if (i >= limit) break;

        cidx = fidx + to.size();
        fidx = str.find(from, cidx);
    }

    return str;
}

#if __has_include(<windows.h>) && __has_include(<winbase.h>)
#include <Shlobj.h>
#endif

/**
 * Expand path-specific keywords, like `~` and `%HOME%`, to
 * its full path, because they are usually not expanded
 */
std::string expand_path(std::string s)
{
#if __has_include(<windows.h>) && __has_include(<winbase.h>)
    WCHAR* path = nullptr;
    std::string homedir = "%HOME%";
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &path))) {
        std::wstring ws(path);
        homedir = std::string(ws.begin(), ws.end());

        CoTaskMemFree(path);
    }

    char* appd = getenv("APPDATA");
    s          = replace_string(s, "%HOME%", homedir);
    s          = replace_string(s, "%APPDATA%", appd ? appd : "%APPDATA%");
    s          = replace_string(s, "\\", "/", MAX_PATH);

#else
    char* homedir = getenv("HOME");
    s             = replace_string(s, "~", homedir);

#endif
    return s;
}

void read_log_block_tags_section(YAML::Node& tags, ConfigData& data)
{
    if (tags.IsSequence()) {
        for (size_t i = 0; i < tags.size(); i++) {
            data.log.blockTags.push_back(tags[i].as<std::string>());
        }        
    }
}

/**
 * Read the log section of the config file
 *
 * The log section looks like this, more or less:
 *
 * ```yaml
 *
 * log:
 * block_tags:
 *   - player_manager
 *   - gl_renderer
 *
 * ```
 *
 */
void read_log_section(YAML::Node& fileinfo, ConfigData& data)
{

    if (fileinfo["block_tags"]) {
        YAML::Node n = fileinfo["block_tags"];
        read_log_block_tags_section(n, data);
    }
}


/**
 * Read the player section of the config file
 *
 * The log section looks like this, more or less:
 *
 * ```yaml
 *
 * player:
 *   username: "Arthur"
 *
 * ```
 *
 */
void read_player_section(YAML::Node& player_info, ConfigData& data)
{
    if (player_info["username"])
        data.player.username = player_info["username"].as<std::string>();

}

bool familyline::read_config_from(std::string_view path, ConfigData& data)
{
    // create a temporary logger, just to capture those values here..
    LoggerService::createLogger(stderr, LogType::Debug, {});
    auto& log = LoggerService::getLogger();

    try {
        YAML::Node config = YAML::LoadFile(std::string{path});

        log->write("config-reader", LogType::Info, "opening file {}", path.data());

        if (config["log"]) {
            YAML::Node n = config["log"];
            read_log_section(n, data);
        }

        if (config["player"]) {
            YAML::Node n = config["player"];
            read_player_section(n, data);
        }
        
        if (config["enable_input_recording"]) {
            data.enableInputRecording = config["enable_input_recording"].as<bool>();
            log->write("config-reader", LogType::Debug, "\tenableInputRecording: {}",
                       data.enableInputRecording);
        }
        
        if (config["default_input_record_directory"]) {
            data.defaultInputRecordDir = expand_path(config["default_input_record_directory"].as<std::string>());
            log->write("config-reader", LogType::Debug, "\tdefaultInputRecordDir: {}",
                       data.defaultInputRecordDir);
        }

        return true;
        
    } catch (YAML::BadFile& b) {
        log->write(
            "config-reader", LogType::Warning,
            "could not open config file '{}', error {} ({})", path.data(), errno,
            strerror(errno));
        return false;
    }

}

#include <fmt/core.h>
#include <client/params.hpp>
#include <cstdlib>

#include <client/graphical/opengl/gl_device.hpp>
#include "client/graphical/device.hpp"

static void show_help()
{
    fmt::print("Familyline help:\n");
    fmt::print("--version:\t\tPrint version and, if compiled inside a Git repo, commit hash\n");
    fmt::print("--help:\t\t\tPrint this help information\n");
    fmt::print("--size <W>x<H>:\t\tChanges the game resolution to <W>x<H> pixels\n");
    fmt::print("--file <path>:\t\tLoad a map from the specified path\n");
    fmt::print(
        "--log [<filename>|screen]: Logs to filename 'filename', or screen to log to screen, or "
        "wherever stderr is bound to\n");
}


static void show_version()
{
    fmt::print("Familyline " VERSION "\n");
    fmt::print("Compiled in " __DATE__ "\n");
    fmt::print("Commit hash " COMMIT "\n");
    fmt::print("Running on {}", SDL_GetPlatform());
    fmt::print("\n");
}

std::tuple<int, int> get_size(std::string_view s) {
    int width=-1, height=-1;

    if (sscanf(s.data(), "%dx%d", &width, &height) <= 1) {
        return std::tuple<int, int>(-1, -1);
    }

    return std::tie(width, height);
}

FILE* get_log_device(std::string_view s) {
    if (s == "stdout") {
        return stdout;
    }

    if (s == "stderr") {
        return stderr;
    }

    FILE* f = fopen(s.data(), "w");
    if (!f) {
        fmt::print("could not open log file '{}': \n", s);
        fmt::print("{} (error {})\n\n", strerror(errno), errno);
        return nullptr;
    }

    return f;
}

std::vector<std::string> get_available_renderers() {
    std::vector<std::string> rs;

    #ifdef RENDERER_OPENGL
    rs.push_back("opengl");
    #endif

    return rs;
}

std::vector<familyline::graphics::Device*> get_device_list(std::string_view renderer) {
    auto rs = get_available_renderers();

    if (rs.empty()) {
        fmt::print("no renderers available!");
        fmt::print("You probably did not compile the game right!");
        std::terminate();
    }

    if (std::find(rs.begin(), rs.end(), renderer) == rs.end()) {
        fmt::print("renderer {} is invalid!", renderer);
        fmt::print("Valid renderers are: ");
        for (auto& r : rs) {
            fmt::print("{}", r);
        }
        fmt::print("\n");
        std::terminate();
    }

#ifdef RENDERER_OPENGL
    if (renderer == "opengl") {
        return familyline::graphics::GLDevice::getDeviceList();
    }
#endif

    return std::vector<familyline::graphics::Device*>{};
}

ParamInfo parse_params(const std::vector<std::string>& params)
{
    ParamInfo pi;
    bool next_is_size = false;
    bool next_is_log = false;
    bool next_is_file = false;

    for (auto& p : params) {
        ////// parse values
        if (next_is_size) {
            std::tie(pi.width, pi.height) = get_size(p);
            next_is_size = false;
            continue;
        }

        if (next_is_log) {
            pi.log_device = get_log_device(p);
            next_is_log = false;
            continue;
        }


        if (next_is_file) {
            pi.mapFile = p;
            next_is_file = false;
            continue;
        }


        ////// parse params

        if (p == "--help") {
            show_help();
            exit(0);
        }

        if (p == "--version") {
            show_version();
            exit(0);
        }

        if (p == "--size") {
            next_is_size = true;
            continue;
        }

        if (p == "--file") {
            next_is_file = true;
            continue;
        }

        if (p == "--log") {
            next_is_log = true;
            continue;
        }


        fmt::print("\t param: {}\n", p);
    }

    /* Treat errors that happen when we expect a value that did not exist */

    if (next_is_size) {
        fmt::print("Expected size in format <width>x<height>\n");
        exit(1);
    }

    if (next_is_log || !pi.log_device) {
        fmt::print("Expected log device: can be stdout, stderr or a filename\n");
        exit(1);
    }

    pi.devices = get_device_list(pi.renderer);
    
    return pi;
}

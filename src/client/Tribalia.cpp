
static int show_starting_menu();




int main(int argc, char const* argv[])
{
    // Start log as early as possible

    FILE* fLog = nullptr;

    int i = get_arg_index("--log", argc, argv);
    if (i >= 0) {
        if ((i + 1) >= argc) {
            fmt::print(stderr, "--log: expected a filename or 'screen'\n");
            return EXIT_FAILURE;
        }

        fmt::print(stderr, "logging in {:s}\n", argv[i + 1]);
        if (!strcmp(argv[i + 1], "screen")) {
            fLog = stderr;
        } else {
            fLog = fopen(argv[i + 1], "a");
            fputs("\n\n", fLog);
        }
        if (!fLog) {
            perror("Could not create log file: ");
            fmt::print(stderr, "Using defaults");
        }
    }

    if (!fLog) {

        } else {
            errno = 0;  // no need to worry
            /* Tries to create tribalia.log, fallback to stderr if it can't */
            fLog = fopen("familyline.log", "a");
            fputs("\n\n\n\n\n\n\n\n\n\n\n", fLog);
            if (!fLog) {
                perror("Could not create log file: ");
                fmt::print(stderr, "Falling back to stderr");
                fLog = stderr;
            }
        }
    }


    // int winW = 640, winH = 480;
    if (get_arg_index("--version", argc, argv) >= 0) {
        show_version();
        return EXIT_SUCCESS;
    }

    i = get_arg_index("--size", argc, argv);
    if (i >= 0) {
        if (check_size(i, argc, argv) < 0) {
            return EXIT_FAILURE;
        }
    }

    if (get_arg_index("--help", argc, argv) >= 0) {
        show_help();
        return EXIT_SUCCESS;
    }


    return show_starting_menu();
}

static void enable_gl_debug();

static int show_starting_menu()
{

}

static void enable_gl_debug()
{
    auto& log = LoggerService::getLogger();
    struct LogTime {
        unsigned qt      = 0;
        unsigned lastsec = 0;
    };

    auto gl_debug_callback = [](GLuint source, GLuint type, unsigned int id, GLuint severity,
                                int length, const char* msg, const void* userparam) {
        (void)userparam;

        /* Handle log suppressing */
        static std::map<unsigned, LogTime> id_qt_map;
        static unsigned lastsupp = (unsigned)-1;
        auto t                   = time(NULL);
        auto& log                = LoggerService::getLogger();

        /* Unban after a long time */
        if (id == lastsupp && id_qt_map[id].lastsec < t + 5) {
            id_qt_map[id].qt = 0;
            return;
        } else {
            lastsupp = -1;
        }

        if (t >= id_qt_map[id].lastsec) {
            id_qt_map[id].qt++;
            id_qt_map[id].lastsec = t;
        }

        if (id_qt_map[id].qt > 20 && id_qt_map[id].lastsec <= t) {
            log->write(
                "gl-debug-output", LogType::Warning,
                "Suppressing id %d messages because they are too many", id);
            lastsupp = id;
            return;
        }

        id_qt_map[id].lastsec = t;

        /* Handle message parsing and display */
        const char *ssource, *stype, *sseverity;
        switch (source) {
            case GL_DEBUG_SOURCE_API: ssource = "gl-debug-opengl-api"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: ssource = "gl-debug-window-system"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: ssource = "gl-debug-shader-compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY: ssource = "gl-debug-third-party"; break;
            case GL_DEBUG_SOURCE_APPLICATION: ssource = "gl-debug-application"; break;
            case GL_DEBUG_SOURCE_OTHER: ssource = "gl-debug-other"; break;
            default: ssource = "gl-debug"; break;
        }

        switch (type) {
            case GL_DEBUG_TYPE_ERROR: stype = "error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: stype = "deprecated behavior"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: stype = "undefined behavior"; break;
            case GL_DEBUG_TYPE_PORTABILITY: stype = "portability issue"; break;
            case GL_DEBUG_TYPE_PERFORMANCE: stype = "performance"; break;
            case GL_DEBUG_TYPE_OTHER: stype = "other"; break;
            default: stype = "unknown"; break;
        }

        LogType ltype = LogType::Info;

        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                sseverity = "";
                ltype     = LogType::Error;
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                ltype     = LogType::Warning;
                sseverity = "";
                break;
            case GL_DEBUG_SEVERITY_LOW:
                ltype     = LogType::Debug;
                sseverity = " low prio: ";
                break;
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                ltype     = LogType::Debug;
                sseverity = "";
                break;
            default: sseverity = " PRIO: ????"; break;
        }

        char* smsg = new char[length + 2];
        memset(smsg, 0, length + 2);
        strncpy(smsg, msg, length);

        fmt::memory_buffer out;
        format_to(out, "[#{:d}] {:s} {:s}: {:s}\0\0", id, sseverity, stype, smsg);

        delete[] smsg;
        LoggerService::getLogger()->write(ssource, ltype, "%s", fmt::to_string(out).data());
    };

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    if (GL_KHR_debug && glDebugMessageCallback) {
        // Try KHR_debug first
        log->write("init", LogType::Info, "KHR_debug supported and used to get GPU debug messages");
        glDebugMessageCallback(gl_debug_callback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    } else if (GLEW_ARB_debug_output && glDebugMessageCallbackARB) {
        // Try ARB_debug_output as a fallback (it's older, but best supported)
        log->write(
            "init", LogType::Info,
            "ARB_debug_output supported and used to get GPU debugging messages");
        /* Create the callback */
        glDebugMessageCallbackARB(gl_debug_callback, nullptr);
        glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    } else {
        log->write(
            "init", LogType::Warning,
            "Neither KHR_debug nor ARB_debug_output extensions are supported");
        log->write("init", LogType::Warning, "GPU debugging messages will not be available");
    }
}

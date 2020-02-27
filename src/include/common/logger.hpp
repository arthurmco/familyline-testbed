#pragma once

/**
 * Familyline logging functions
 *
 * Copyright (C) 2020 Arthur Mendes
 */

#include <cstdio>
#include <chrono>
#include <memory>

namespace familyline {

#define LOGDEBUG(log, tag, format, ...) log->write(tag, LogType::Debug, format, __VA_ARGS__)
    
    enum LogType {
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    class Logger {

    private:
        FILE* _out = nullptr;

        std::chrono::steady_clock::time_point _start;
        double getDelta();

    public:
        Logger(FILE* out = stderr)
            : _out(out), _start(std::chrono::steady_clock::now())
            {}

        void write(const char* tag, LogType type, const char* format, ...);
    };

    class LoggerService {
    private:
        static std::unique_ptr<Logger> _logger;

    public: 
        static void createLogger(FILE* out = stderr)
            {
                _logger = std::make_unique<Logger>(out);
            }

        static std::unique_ptr<Logger>& getLogger()
            {
                return _logger;
            }
    };

}

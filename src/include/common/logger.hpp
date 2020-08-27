#pragma once

/**
 * Familyline logging functions
 *
 * Copyright (C) 2020 Arthur Mendes
 */

#include <chrono>
#include <cstdio>
#include <memory>
#include <mutex>

namespace familyline
{
#define LOGDEBUG(log, tag, format, ...) log->write(tag, LogType::Debug, format, __VA_ARGS__)

enum LogType { Debug, Info, Warning, Error, Fatal };

class Logger
{
private:
    FILE* _out = nullptr;
    LogType _minlog;

    std::chrono::steady_clock::time_point _start;
    double getDelta();

    std::mutex log_mtx;

public:
    Logger(FILE* out = stderr, LogType minlog = LogType::Info)
        : _out(out), _minlog(minlog), _start(std::chrono::steady_clock::now())
    {
    }

    void write(const char* tag, LogType type, const char* format, ...);
};

class LoggerService
{
private:
    static std::unique_ptr<Logger> _logger;

public:
    static void createLogger(FILE* out = stderr, LogType minlog = LogType::Debug)
    {
        _logger = std::make_unique<Logger>(out, minlog);
    }

    static std::unique_ptr<Logger>& getLogger() { return _logger; }
};

}  // namespace familyline

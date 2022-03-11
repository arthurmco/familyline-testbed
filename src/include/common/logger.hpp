#pragma once

/**
 * Familyline logging functions
 *
 * Copyright (C) 2020 Arthur Mendes
 */

#include <chrono>
#include <cstdio>
#include <memory>
#include <type_traits>
#include <vector>
#include <string>
#include <mutex>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/chrono.h> // so you can print dates to the log
#include <fmt/ranges.h> // so you can directly print ranges to the log

#include <optional>

/// BEGIN custom formatters

#include <glm/glm.hpp>

#include <s7.h>

template <>
struct fmt::formatter<glm::vec3> : formatter<double> {

    template <typename FormatContext>
    auto format(const glm::vec3& v, FormatContext& ctx) {
        format_to(ctx.out(), "(");
        formatter<double>::format(v.x, ctx); 
        format_to(ctx.out(), ", ");
        formatter<double>::format(v.y, ctx);
        format_to(ctx.out(), ", ");
        formatter<double>::format(v.z, ctx);
        return format_to(ctx.out(), ")");
    }   
};

using s7_print_pair = std::pair<s7_scheme*, s7_pointer>;

template <>
struct fmt::formatter<s7_print_pair> : formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const s7_print_pair& s, FormatContext& ctx) {
        std::string v = fmt::format(
            "({})", s7_object_to_c_string(s.first, s.second));
        return format_to(ctx.out(), v);
    }   
};

template <>
struct fmt::formatter<glm::vec2> : formatter<double> {

    template <typename FormatContext>
    auto format(const glm::vec2& v, FormatContext& ctx) {
        format_to(ctx.out(), "(");
        formatter<double>::format(v.x, ctx); 
        format_to(ctx.out(), ", ");
        formatter<double>::format(v.y, ctx);
        return format_to(ctx.out(), ")");
    }   
};


/**
 * A formatter for the optional type
 *
 * So you can directly log an optional type
 */
template <typename T>
struct fmt::formatter<std::optional<T>> : formatter<T> {

    template <typename FormatContext>
    auto format(const std::optional<T>& v, FormatContext& ctx) {
        if (v) {
            format_to(ctx.out(), "Some(");
            formatter<T>::format(v.value(), ctx);
            return format_to(ctx.out(), ")");
        } else {
            return format_to(ctx.out(), "None");
        }
    }   
};


/// END custom formatters

namespace familyline
{
#define LOGDEBUG(log, tag, format, ...) log->write(tag, LogType::Debug, format, __VA_ARGS__)

enum LogType { Debug, Info, Warning, Error, Fatal };

class Logger
{
private:
    FILE* _out = nullptr;
    LogType minlog_;
    std::vector<std::string> blockTags_;
    std::chrono::steady_clock::time_point _start;
    double getDelta();

    std::mutex mtx;
    const std::string getLevelText(const LogType type);
    
public:
    Logger(
        FILE* out = stderr, LogType minlog = LogType::Info, std::vector<std::string> blockTags = {})
        : _out(out),
          minlog_(minlog),
          _start(std::chrono::steady_clock::now()),
          blockTags_(blockTags)
    {
    }

    /**
     * Well..., write a message to a logfile.
     * The logfile can be a file, or stderr
     *
     * The logging format of this function (the `fmt` parameter) follows the convention
     * of the libfmt library, or the <format> header, which follow similar conventions
     * to the Python `str.format()` function
     *
     * We have 5 levels of logging: Debug, Info, Warning, Error and Fatal.
     * The logging levels are autodescriptive
     *
     */
    template <typename ...Args>
    void write(std::string_view tag, const LogType type, const std::string fmt, Args ...args) {
        if (type < minlog_)
            return;

        if (std::find(blockTags_.begin(), blockTags_.end(), tag) != blockTags_.end()) return;

        auto strtype = this->getLevelText(type);
        auto delta = this->getDelta();
        auto strend = strtype.size() > 0 ? "\033[0m" : "";
        
        auto data = fmt::format(fmt, args...);
        auto stag = tag.size() > 0 ?
            fmt::format("\033[1m{}\033[0m: ", tag) :
            "";
        
        auto msg = fmt::format("[{:13.4f}] {}{}{}{}\n", delta, strtype, stag, data, strend);
        
        mtx.lock();
        fputs(msg.c_str(), _out);
        mtx.unlock();
    }    
};

class LoggerService
{
private:
    static std::unique_ptr<Logger> _logger;

public:
    static void createLogger(
        FILE* out = stderr, LogType minlog = LogType::Debug,
        std::vector<std::string> blockTags = {})
    {
        _logger = std::make_unique<Logger>(out, minlog, blockTags);
    }

    static std::unique_ptr<Logger>& getLogger() { return _logger; }
};

}  // namespace familyline

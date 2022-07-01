#include <algorithm>
#include <common/logger.hpp>
#include <cstdarg>

using namespace familyline;
using namespace std::chrono;

std::unique_ptr<Logger> LoggerService::_logger;

const std::string Logger::getLevelText(const LogType type)
{
    switch (type) {
        case LogType::Debug: return "\033[3m[debug] ";
        case LogType::Info: return "";
        case LogType::Warning: return "\033[33m[WARN] ";
        case LogType::Error: return "\033[31m[ERROR] ";
        case LogType::Fatal: return "\033[31;1m[FATAL] ";
        default: return "";
    }
}

double Logger::getDelta()
{
    decltype(_start) now = steady_clock::now();
    auto deltatime       = duration_cast<duration<double>>(now - _start);
    return deltatime.count();
}

#include <common/logger.hpp>
#include <mutex>
#include <cstdarg>

using namespace familyline;

std::unique_ptr<Logger> LoggerService::_logger;

std::mutex mtx;


void Logger::write(const char* tag, LogType type, const char* format, ...)
{

	const char* prefix;
    const char* colorstart;
    const char* colorend = "\033[0m";
	
	switch (type) {
	case LogType::Debug:
        prefix = "[debug] ";
        colorstart = "\033[3m";
        break;
	case LogType::Info:
        prefix = "";
        colorstart = "";
        break;
	case LogType::Warning:
        prefix = "[warn] ";
        colorstart = "\033[33m";
        break;
	case LogType::Error:
        prefix = "[error] ";
        colorstart = "\033[31m";
        break;
	case LogType::Fatal:
        prefix = "[fatal] ";
        colorstart = "\033[31;1m";
        break;
	default:
        prefix = "";
        break;
	}

	char msg[1024] = {};
	
	va_list vl;
	va_start(vl, format);

	vsnprintf(msg, 512, format, vl);

	va_end(vl);

    mtx.lock();

    if (!tag || tag[0] == '\0')
        fprintf(_out, "%s%s%s%s\n", colorstart, prefix, msg, colorend);
    else
        fprintf(_out, "%s%s%s: %s%s\n", colorstart, prefix, tag, msg, colorend);
    
    mtx.unlock();
}

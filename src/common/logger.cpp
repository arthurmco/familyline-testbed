#include <common/logger.hpp>
#include <mutex>
#include <cstdarg>

using namespace familyline;

std::unique_ptr<Logger> LoggerService::_logger;

std::mutex mtx;


void Logger::write(const char* tag, LogType type, const char* format, ...)
{

	const char* prefix;

	switch (type) {
	case LogType::Debug: prefix = "[debug] "; break;
	case LogType::Info: prefix = ""; break;
	case LogType::Warning: prefix = "[warn] "; break;
	case LogType::Error: prefix = "[error] "; break;
	case LogType::Fatal: prefix = "[fatal] "; break;
	default: prefix = ""; break;
	}

	char msg[1024] = {};
	
	va_list vl;
	va_start(vl, format);

	vsnprintf(msg, 512, format, vl);

	va_end(vl);

    mtx.lock();

    if (!tag || tag[0] == '\0')
        fprintf(_out, "%s%s\n", prefix, msg);
    else
        fprintf(_out, "%s%s: %s\n", prefix, tag, msg);
    
    mtx.unlock();
}

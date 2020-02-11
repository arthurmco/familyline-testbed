#include <client/graphical/logger.hpp>

#include <cstdarg>

std::unique_ptr<Logger> LoggerService::_logger;

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

	char msg[512];
	
	va_list vl;
	va_start(vl, format);

	vsnprintf(msg, 512, format, vl);

	va_end(vl);

	fprintf(_out, "%s%s: %s\n", prefix, tag, msg);
}

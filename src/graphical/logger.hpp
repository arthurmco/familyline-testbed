#pragma once

#include <cstdio>
#include <memory>

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

public:
	Logger(FILE* out = stderr)
		: _out(out)
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
/***
    Logging functions for Familyline

    Copyright (C) 2016, 2017 Arthur M.

***/

#include <cstdio>
#include <ctime>
#include <cstdarg>
#include <chrono>

#ifndef LOG_HPP
#define LOG_HPP

namespace familyline {

    class Log {
    private:

        FILE* _logFile = nullptr;
	std::chrono::steady_clock::time_point start;
        static Log* l;
	double GetDelta();

	const char* L_DEBUG = "\033[3m";
	const char* L_BOLD = "\033[1m";
	const char* L_RED = "\033[31m";
	const char* L_BOLDRED = "\033[31;1m";
	const char* L_YELLOW = "\033[33m";
	const char* L_BOLDYELLOW = "\033[33;1m";
	const char* L_NORMAL = "\033[0m";
	
    public:
        Log();
	
	void SetFile(FILE* f);
	void InfoWrite(const char* tag, const char* fmt, ...);
	void Write(const char* tag, const char* fmt, ...);
	void Warning(const char* tag, const char* fmt, ...);
	void Fatal(const char* tag, const char* fmt, ...);

         ~Log();

         static Log* GetLog();
    };

}


#endif /* end of include guard: LOG_HPP */

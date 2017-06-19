/***
    Logging functions for Tribalia

    Copyright (C) 2016, 2017 Arthur M.

***/

#include <cstdio>
#include <ctime>
#include <cstdarg>
#include <chrono>

#ifndef LOG_HPP
#define LOG_HPP

namespace Tribalia {

    class Log {
    private:

        FILE* _logFile = nullptr;
	std::chrono::steady_clock::time_point start;
        static Log* l;
	double GetDelta();

    public:
        Log();
	
	void SetFile(FILE* f);
	void Write(const char* tag, const char* fmt, ...);
	void Warning(const char* tag, const char* fmt, ...);
	void Fatal(const char* tag, const char* fmt, ...);

         ~Log();

         static Log* GetLog();
    };

}


#endif /* end of include guard: LOG_HPP */

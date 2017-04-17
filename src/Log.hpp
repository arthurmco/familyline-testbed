/***
    Logging functions for Tribalia

    Copyright (C) 2016 Arthur M.

***/

#include <cstdio>
#include <ctime>
#include <cstdarg>

#ifndef LOG_HPP
#define LOG_HPP

namespace Tribalia {

    class Log {
    private:

        FILE* _logFile = nullptr;
        time_t _time;
        static Log* l;

    public:
        Log();


         void SetFile(FILE* f);
         void Write(const char* fmt, ...);
         void Warning(const char* fmt, ...);
         void Fatal(const char* fmt, ...);

         ~Log();

         static Log* GetLog();
    };

}


#endif /* end of include guard: LOG_HPP */

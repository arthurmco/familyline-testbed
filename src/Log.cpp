#include "Log.hpp"

using namespace Tribalia;
using namespace std::chrono;

Log* Log::l = NULL;

Log* Log::GetLog(){
    if (!l){
        l = new Log();
    }

    return l;
}

Log::Log()
{
    start = steady_clock::now();
}

void Log::SetFile(FILE* f)
{
    if (_logFile){
        fflush(this->_logFile);
        fclose(this->_logFile);
    }

    this->_logFile = f;
}

#define BOLD "\033[1m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define NORMAL "\033[0m"

double Log::GetDelta() {
    decltype(start) now = steady_clock::now();
    auto deltatime = duration_cast<duration<double>>(now-start);
    return deltatime.count();
}

void Log::Write(const char* tag, const char* fmt, ...)
{
    if (!this->_logFile) return;

    const char* colon = (tag[0] == '\0') ? "" : ":";
    
    /* Print timestamp */
    
    fprintf(_logFile, "[%13.4f] " BOLD "%s%s " NORMAL, GetDelta(), tag, colon);

    /* Print message */
    va_list vl;
    va_start(vl, fmt);

    vfprintf(_logFile, fmt, vl);

    va_end(vl);

    /* Print line terminator */
    fputs(NORMAL "\r\n", _logFile);
    fflush(this->_logFile);
}

void Log::Fatal(const char* tag, const char* fmt, ...)
{
    if (!this->_logFile) return;
    
    /* Print timestamp */
    fprintf(_logFile, "[%13.4f] " RED "" BOLD "%s: (FATAL) " 
	    NORMAL "" RED, GetDelta(), tag);

    /* Print message */
    va_list vl;
    va_start(vl, fmt);

    vfprintf(_logFile, fmt, vl);

    va_end(vl);

    /* Print line terminator */
    fputs(NORMAL "\r\n", _logFile);
    fflush(this->_logFile);
}

void Log::Warning(const char* tag, const char* fmt, ...)
{
    if (!this->_logFile) return;
	
    /* Print timestamp */
    fprintf(_logFile, "[%13.4f] " YELLOW "" BOLD "%s: (WARNING) " 
	    NORMAL "" YELLOW, GetDelta(), tag);

    /* Print message */
    va_list vl;
    va_start(vl, fmt);

    vfprintf(_logFile, fmt, vl);

    va_end(vl);

    /* Print line terminator */
    fputs(NORMAL "\r\n", _logFile);
    fflush(this->_logFile);
}

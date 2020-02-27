#include <common/Log.hpp>

#ifdef WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

#include <cstring> // strcmp
#include <cstdlib> // getenv
#include <mutex>

using namespace familyline;
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

    if (isatty(fileno(f))) {
	if (getenv("TERM") &&
		!strcmp("xterm-256color", getenv("TERM"))) {
	    L_BOLD = "\033[1m";  
	    L_RED = "\033[31m";
	    L_BOLDRED = "\033[31;1m";  
	    L_YELLOW = "\033[38;5;220m";
	    L_BOLDYELLOW = "\033[38;5;220;1m";
	    L_NORMAL = "\033[0m";
	    L_DEBUG = "\033[3m";
	} else {
	    L_BOLD = "\033[1m";  
	    L_RED = "\033[31m";
	    L_BOLDRED = "\033[31;1m";  
	    L_YELLOW = "\033[33m";
	    L_BOLDYELLOW = "\033[33;1m";
	    L_NORMAL = "\033[0m";
	    L_DEBUG = "\033[1;30m";
	}
    } else {
	L_BOLD = "";  
	L_RED = "";
	L_BOLDRED  = "";
	L_YELLOW = "";
	L_BOLDYELLOW = "";
	L_NORMAL = "";
	L_DEBUG = "";
    }
    

    this->_logFile = f;
}

double Log::GetDelta() {
    decltype(start) now = steady_clock::now();
    auto deltatime = duration_cast<duration<double>>(now-start);
    return deltatime.count();
}

void Log::InfoWrite(const char* tag, const char* fmt, ...) {
    if (!this->_logFile) return;

	mtx.lock();
    const char* colon = (tag[0] == '\0') ? "" : ":";
    
    /* Print timestamp */
    
    fprintf(_logFile, "[%13.4f] %s%s%s ", GetDelta(), tag, colon,
	L_DEBUG);

    /* Print message */
    va_list vl;
    va_start(vl, fmt);

    vfprintf(_logFile, fmt, vl);

    va_end(vl);

    /* Print line terminator */
    fprintf(_logFile, "%s\r\n", L_NORMAL);
    fflush(this->_logFile);
	mtx.unlock();
}

void Log::Write(const char* tag, const char* fmt, ...)
{
    if (!this->_logFile) return;

	mtx.lock();
    const char* colon = (tag[0] == '\0') ? "" : ":";
    
    /* Print timestamp */
    
    fprintf(_logFile, "[%13.4f] %s%s%s%s ", GetDelta(), L_BOLD, tag, colon,
	L_NORMAL);

    /* Print message */
    va_list vl;
    va_start(vl, fmt);

    vfprintf(_logFile, fmt, vl);

    va_end(vl);

    /* Print line terminator */
    fprintf(_logFile, "%s\r\n", L_NORMAL);
    fflush(this->_logFile);
	mtx.unlock();
}

void Log::Fatal(const char* tag, const char* fmt, ...)
{
    if (!this->_logFile) return;
    
	mtx.lock();
    /* Print timestamp */
    fprintf(_logFile, "[%13.4f] %s%s: (FATAL) %s%s", 
	    GetDelta(), L_BOLDRED, tag, L_NORMAL, L_RED);

    /* Print message */
    va_list vl;
    va_start(vl, fmt);

    vfprintf(_logFile, fmt, vl);

    va_end(vl);

    /* Print line terminator */
    fprintf(_logFile, "%s\r\n", L_NORMAL);
    fflush(this->_logFile);
	mtx.unlock();
}

void Log::Warning(const char* tag, const char* fmt, ...)
{
    if (!this->_logFile) return;
	
	mtx.lock();
    /* Print timestamp */
    fprintf(_logFile, "[%13.4f] %s%s: (WARNING) %s%s",
	    GetDelta(), L_BOLDYELLOW, tag, L_NORMAL, L_YELLOW);

    /* Print message */
    va_list vl;
    va_start(vl, fmt);

    vfprintf(_logFile, fmt, vl);

    va_end(vl);

    /* Print line terminator */
    fprintf(_logFile, "%s\r\n", L_NORMAL);
    fflush(this->_logFile);
	mtx.unlock();
}

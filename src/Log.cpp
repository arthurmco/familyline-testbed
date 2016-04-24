#include "Log.hpp"

using namespace Tribalia;

Log* Log::l = NULL;

Log* Log::GetLog(){
    if (!l){
        l = new Log();
    }

    return l;
}

Log::Log()
{
    time(&_time);
}

void Log::SetFile(FILE* f)
{
    if (_logFile){
        fflush(this->_logFile);
        fclose(this->_logFile);
    }

    this->_logFile = f;
}
void Log::Write(const char* fmt, ...)
{
    /* Print timestamp */
    time_t now;
    time(&now);
    fprintf(_logFile, "[%8u] ", (int)(now-_time));

    /* Print message */
    va_list vl;
    va_start(vl, fmt);

    vfprintf(_logFile, fmt, vl);

    va_end(vl);

    /* Print line terminator */
    fputs("\r\n", _logFile);
}

void Log::Fatal(const char* fmt, ...)
{
    /* Print timestamp */
    time_t now;
    time(&now);
    fprintf(_logFile, "[%8u] (FATAL) ", (int)(now-_time));

    /* Print message */
    va_list vl;
    va_start(vl, fmt);

    vfprintf(_logFile, fmt, vl);

    va_end(vl);

    /* Print line terminator */
    fputs("\r\n", _logFile);
}

void Log::Warning(const char* fmt, ...)
{
    /* Print timestamp */
    time_t now;
    time(&now);
    fprintf(_logFile, "[%8u] (WARNING) ", (int)(now-_time));

    /* Print message */
    va_list vl;
    va_start(vl, fmt);

    vfprintf(_logFile, fmt, vl);

    va_end(vl);

    /* Print line terminator */
    fputs("\r\n", _logFile);
}

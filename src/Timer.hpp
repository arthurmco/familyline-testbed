/***
    Timer control functions for Familyline

    Copyright (C) 2016 Arthur Mendes.

***/

#include <cstdio>
#include <list>
#include <map>

#include "Log.hpp"

#ifndef TIMER_HPP
#define TIMER_HPP

namespace Familyline {

/* The function pointer for a timer event
    - opt_inst is the optional instance argument, automatically used for C++
    when you call some class function. If you are calling a class function, you
    need to omit this
    - frame_delta is the time that the actual game loop iteration has taken
    - opt is an optional argument you've chosen
*/
typedef void (*TimerFunction)(void* opt_inst, double frame_delta, void* opt);

struct TimerFunctionStruct {
    int eid;
    void* opt_inst;
    void* opt_arg;
    TimerFunction func;
};

struct TimerEvent {
    double ms_sum;
    std::list<TimerFunctionStruct> funcs;
    bool was_run = false;
};

class Timer {
private:
    int max_id;
    std::map<double /*milisecond*/, TimerEvent> _function_events;

    double ms_sum;

public:
    Timer();


    /* Add a function to be run every 'ms' miliseconds. 
        opt_inst is the class instance of that function, can be set to nullptr if the function is
        static, lambda or a C function
        opt_arg is some optional argument you want to pass 
        
        Returns the timer event ID.
        */
    int AddFunctionCall(double ms, TimerFunction f, void* opt_inst, void* opt_arg);

    void RunTimers(double msdelta);

    static Timer* getInstance() {
        static Timer* t = nullptr;
        if (!t) t = new Timer;

        return t;
    }
};
}

#endif
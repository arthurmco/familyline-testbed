#include "Timer.hpp"
using namespace familyline;

Timer::Timer()
{
    max_id = 0;
    ms_sum = 0;
}


/* Add a function to be run every 'ms' miliseconds. 
   opt_inst is the class instance of that function, can be set to nullptr if the function is
   static, lambda or a C function
   opt_arg is some optional argument you want to pass 
   
   Returns the timer event ID.
   */
int Timer::AddFunctionCall(double ms, TimerFunction f, void* opt_inst, void* opt_arg)
{
    auto it_evt = _function_events.find(ms);
    Log::GetLog()->Write("timer", "Added function @%#p with instance %#p, arg %#p at each %.1f ms",
        f, opt_inst, opt_arg, ms);


    TimerFunctionStruct tfs;
    tfs.eid = max_id++;
    tfs.opt_inst = opt_inst;
    tfs.opt_arg = opt_arg;
    tfs.func = f;

    if (it_evt != _function_events.end()) {
        it_evt->second.funcs.push_back(tfs);
    } else {
        _function_events[ms].ms_sum = 0;
        _function_events[ms].funcs.push_back(tfs);
    }

    return tfs.eid;
}


void Timer::RunTimers(double msdelta)
{
    ms_sum += msdelta;

    for (auto& tevs : _function_events) {
        if (tevs.second.was_run) {
            tevs.second.was_run = false;
        }

        // printf("global: %.3f ms, local (%.2f) : %.3f ms\n", 
        //     ms_sum, tevs.first, tevs.second.ms_sum);
        tevs.second.ms_sum += msdelta;

        /*  Run the timer event that it needs to be runned.
            Please note that the called function needs to be aware of
            delta time distortions.
        */
        if (tevs.second.ms_sum >= tevs.first) {
            for (auto& funcs : tevs.second.funcs) {
                // printf("\t calling function %#p (args %#x %.3f %#x)\n", funcs.func, 
                //     funcs.opt_inst, tevs.second.ms_sum, funcs.opt_arg);
                funcs.func(funcs.opt_inst, tevs.second.ms_sum, funcs.opt_arg);
            }
            tevs.second.ms_sum = 0;
            tevs.second.was_run = true;
        }
        
    }
}

#include <common/logic/script_environment.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <common/logger.hpp>
#include <filesystem>
#include <s7.h>

using namespace familyline::logic;

std::vector<ScriptEnvironment::EnvironmentInfo> ScriptEnvironment::environments = {};


template <typename T>
std::optional<T> ScriptEnvironment::convertTypeFrom(s7_scheme *s7,
                                                    s7_pointer value) { return std::nullopt; }

template <>
std::optional<long> ScriptEnvironment::convertTypeFrom(s7_scheme *s7,
                                                       s7_pointer value) {    
    return s7_is_integer(value) ? std::make_optional(s7_integer(value)) : std::nullopt ;
}

template <>
std::optional<unsigned long> ScriptEnvironment::convertTypeFrom(s7_scheme *s7,
                                                                s7_pointer value) {    
    return s7_is_integer(value) ? std::make_optional(s7_integer(value)) : std::nullopt ;
}

template <>
std::optional<unsigned> ScriptEnvironment::convertTypeFrom(s7_scheme *s7,
                                                           s7_pointer value) {    
    return s7_is_integer(value) ? std::make_optional(s7_integer(value)) : std::nullopt ;
}

template <>
std::optional<std::string> ScriptEnvironment::convertTypeFrom(s7_scheme *s7,
                                                              s7_pointer value) {    
    return s7_is_string(value) ? std::make_optional(s7_string(value)) : std::nullopt ;
}

template <>
std::optional<bool> ScriptEnvironment::convertTypeFrom(s7_scheme *s7,
                                                       s7_pointer value) {    
    return s7_is_boolean(value) ?
        std::make_optional(s7_boolean(s7, value)) :
        std::nullopt ;
}

/**
 * Execute a certain script file
 */
void ScriptEnvironment::runScript(std::string path)
{
    auto &log  = familyline::LoggerService::getLogger();
    log->write("script-environment", LogType::Info, "loading script file {}", path);

    s7_load(s7_, path.c_str());
}

s7_pointer ScriptEnvironment::evalFunction(std::string name, s7_pointer arg)
{
    auto &log  = familyline::LoggerService::getLogger();
    log->write("script-environment", LogType::Info, "calling function {}", name);
    
    s7_pointer fn = s7_name_to_value(s7_, name.c_str());
    log->write("script-environment", LogType::Info, "!!! <{}>", std::make_pair(s7_, fn));
            
    if (!s7_is_procedure(fn)) {
        log->write("script-environment", LogType::Info, "cannot call {}: wrong type or does not exist",
                   name);
        return s7_f(s7_);
    }
    
    return s7_call(s7_, fn, s7_cons(s7_, arg, s7_nil(s7_)));
}



/**
 * Call a public function
 *
 * On Scheme, you call this function like this:
 *  (call-public 'function-name param)
 */
s7_pointer ScriptEnvironment::callPublicFunctionOnEnv(s7_scheme* sc, s7_pointer args)
{
    s7_pointer functionName = s7_car(args);
    s7_pointer param = s7_cadr(args);
    
    
    auto &log  = familyline::LoggerService::getLogger();

    
    if (!s7_is_symbol(functionName)) {
        log->write("script-environment", LogType::Error,
                   "invalid type for function-name");
        return s7_f(sc);
    }

    auto fnName = std::string(s7_symbol_name(functionName));
    s7_pointer current_global           = s7_eval_c_string(sc, "current-global-env");
    uintptr_t current_global_ptr = (uintptr_t)s7_integer(current_global);

    auto v = std::find_if(
        ScriptEnvironment::environments.begin(), ScriptEnvironment::environments.end(),
        [&](auto& v) {
            if (v.bcanary[0] == 0x72637345 && ((uintptr_t)v.env) == current_global_ptr &&
                v.ecanary[0] == 0x20434e50 && v.ecanary[5] == 0x0) {
                return true;
            }

            return false;
        });

    if (v == ScriptEnvironment::environments.end()) {
        throw std::runtime_error("Invalid script environment!");
    }

    return v->env->callPublicFunction(fnName, param);
}

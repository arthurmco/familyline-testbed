#include <common/logic/script_environment.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <common/logger.hpp>

using namespace familyline::logic;

std::vector<ScriptEnvironment::EnvironmentInfo> ScriptEnvironment::environments = {};


template <typename T>
std::optional<T> ScriptEnvironment::convertTypeFrom(SCM value) { return std::nullopt; }

template <>
std::optional<long> ScriptEnvironment::convertTypeFrom(SCM value) {    
    return scm_is_integer(value) ? std::make_optional(scm_to_long(value)) : std::nullopt ;
}

template <>
std::optional<unsigned long> ScriptEnvironment::convertTypeFrom(SCM value) {    
    return scm_is_integer(value) ? std::make_optional(scm_to_ulong(value)) : std::nullopt ;
}

template <>
std::optional<unsigned> ScriptEnvironment::convertTypeFrom(SCM value) {    
    return scm_is_integer(value) ? std::make_optional(scm_to_uint(value)) : std::nullopt ;
}

template <>
std::optional<std::string> ScriptEnvironment::convertTypeFrom(SCM value) {    
    return scm_is_string(value) ? std::make_optional(scm_to_locale_string(value)) : std::nullopt ;
}

/**
 * Execute a certain script file
 */
void ScriptEnvironment::runScript(std::string path)
{
    auto &log  = familyline::LoggerService::getLogger();
    log->write("script-environment", LogType::Info, "loading script file {}", path);
    
    scm_c_define("current-global-env", scm_from_unsigned_integer((uintptr_t)this));
    scm_c_primitive_load(path.c_str());
}

SCM ScriptEnvironment::evalFunction(std::string name, SCM arg)
{
    auto &log  = familyline::LoggerService::getLogger();
    log->write("script-environment", LogType::Info, "calling function {}", name);
    
    SCM fn = scm_variable_ref(scm_c_lookup(name.c_str()));
    log->write("script-environment", LogType::Info, "!!! <{}>", fn);
            
    if (scm_procedure_p(fn) == SCM_BOOL_F) {
        log->write("script-environment", LogType::Info, "cannot call {}: wrong type or does not exist",
                   name);
        return SCM_BOOL_F;
    }
    
    scm_c_define("current-global-env", scm_from_unsigned_integer((uintptr_t)this));
    return scm_call_1(fn, arg);
}



/**
 * Call a public function
 *
 * On Scheme, you call this function like this:
 *  (call-public 'function-name param)
 */
SCM ScriptEnvironment::callPublicFunctionOnEnv(SCM functionName, SCM param)
{
    auto &log  = familyline::LoggerService::getLogger();

    auto fnName = ScriptEnvironment::convertTypeFrom<std::string>(scm_symbol_to_string(functionName));
    if (!fnName) {
        log->write("script-environment", LogType::Error,
                   "invalid type for function-name");
        return SCM_BOOL_F;
    }

    SCM current_global           = scm_c_eval_string("current-global-env");
    uintptr_t current_global_ptr = scm_to_uintptr_t(current_global);

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

    return v->env->callPublicFunction(*fnName, param);
}

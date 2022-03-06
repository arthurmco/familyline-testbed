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



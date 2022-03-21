#pragma once

/**
 * Provides a way of binding native functions to scripts, and to
 * running them.
 *
 * Copyright (C) 2022 Arthur Mendes
 */

#include <s7.h>

#include <any>
#include <optional>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <functional>

namespace familyline::logic
{
/**
 * An environment for scheme script files to run
 *
 * Provides a nicer way of running scripts and adding native functions to the
 * scheme environment.
 *
 * It is nice to defer some things to scripts, because this means more
 * flexibility, and lower compile times.
 *
 * Most scheme implementation does not allow an implicit `data*` parameter
 * where we can pass our context
 * For a programmer point of view, this is cool, but these script files
 * favor practicity first, and it is more practical to allow a "global"
 * context.
 */
class ScriptEnvironment
{
public:
    template <typename T>
    ScriptEnvironment(T globalv) : globalv_(std::any(globalv))
    {
        EnvironmentInfo e;
        e.env = this;
        environments.push_back(e);

        s7_ = s7_init();
        s7_define_function(s7_, "call-public", (s7_function)&ScriptEnvironment::callPublicFunctionOnEnv,
                           1, 1, false, "(call-public function args) call a 'public' function" );
        s7_define_variable(s7_,  "current-global-env",
                           s7_make_integer(s7_, (uintptr_t)this));
    }

    ~ScriptEnvironment() {
        
        puts("TODO: delete the environment from environments list");
    }

    ScriptEnvironment(ScriptEnvironment& other)       = delete;
    ScriptEnvironment(const ScriptEnvironment& other) = delete;

    /**
     * The environment information struct
     *
     * Houses the pointer to an environment, plus canaries so that we
     * can detect corruption
     *
     * TODO: make this safer!
     */
    struct EnvironmentInfo {
        unsigned int bcanary[6] = {0x72637345, 0x20697665, 0x61732065,
                                   0x6f632069, 0x6e657272, 0x6f64};
        ScriptEnvironment* env;
        unsigned int ecanary[6] = {0x20434e50, 0x71206564, 0x206d6575, 0x6c206174, 0x6f646e65, 0x0};
    };

    
    /**
     * This is a little quirk to support embedded scheme with global state,
     * such as s7, with our local scheme state.
     *
     * We define a list of defined environments, and store a pointer of the current
     * environment on each run
     */
    static std::vector<EnvironmentInfo> environments;
    /**
     * Register a native function to be used in the script
     *
     * `name` is its name on the script
     * `params` is the parameter count
     * `fptr` is a pointer to it
     *
     * You will be able to call this function from the script.
     * All arguments, however, will be passed as an s7_pointer object, which is
     * nothing more than a scheme atom. You will have to do the conversion back
     * to the type you want, but we have the conversion functions available.
     *
     * This function is very basic. It does not automatically typecheck
     * arguments, nor it sends the global environment for you.
     */
    template <typename R, typename... Args>
    void registerFunction(std::string name, unsigned params, R (*fptr)(Args...))
    {
        s7_define_function(s7_, name.c_str(), (s7_function)fptr,
                           params, 0, false, "");
    }


    /**
     * Register a public function
     *
     * The difference between a native and a public function is that
     * the public function can be any callable. This may cause things
     * to go a bit slow on the translation, because s7 only allow
     * us to call functions, not any callable, such as functors and lambdas.
     *
     * The same rules apply however, and you will need to convert them
     * arguments, too.
     *
     * Also, the public function only receives one argument.
     */
    void registerPublicFunction(std::string name, std::function<s7_pointer(s7_scheme*,s7_pointer)> fun)
    {
        public_fns_[name] = fun;
    }

    s7_pointer callPublicFunction(std::string name, s7_pointer param) {
        if (!public_fns_.contains(name))
            return s7_f(s7_);

        return public_fns_[name](s7_, param);
    }
    
    /**
     * Execute a certain script file
     */
    void runScript(std::string path);

    template <typename T>
    T get_value()
    {
        return std::any_cast<T>(globalv_);
    }

    template <typename T>
    static T getGlobalEnv(s7_scheme* sc)
    {
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

        return v->env->template get_value<T>();
    }

    static s7_pointer callPublicFunctionOnEnv(s7_scheme* sc, s7_pointer args);    
    /**
     * Convert an scheme atom to a concrete type (that you specify)
     *
     * If the type conversion fails, returns nullopt.
     */
    template <typename T>
    static std::optional<T> convertTypeFrom(s7_scheme *s7, s7_pointer value);

    s7_pointer evalFunction(std::string name, s7_pointer arg);
    s7_scheme* getContext() { return s7_; }

private:
    std::any globalv_;
    s7_pointer global_scm_t_;
    s7_scheme *s7_;
    
    std::unordered_map<std::string, std::function<s7_pointer(s7_scheme*,s7_pointer)>> public_fns_;
};  // namespace familyline::logic

}  // namespace familyline::logic

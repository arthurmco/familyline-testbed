#include <gtest/gtest.h>

#include <array>
#include <common/logic/script_environment.hpp>
#include <memory>

#include "utils.hpp"

using namespace familyline::logic;

SCM hello_world(SCM value)
{
    long* env = ScriptEnvironment::getGlobalEnv<long*>();
    *env      = ScriptEnvironment::convertTypeFrom<long>(value).value();
    return SCM_BOOL_T;
}

SCM get_value() { return scm_from_signed_integer(5678); }

SCM set_value(SCM value)
{
    long* env = ScriptEnvironment::getGlobalEnv<long*>();
    *env      = ScriptEnvironment::convertTypeFrom<long>(value).value();
    return SCM_BOOL_T;
}

TEST(ScriptInterpreter, TestSimpleFunctionCall)
{
    scm_init_guile();
    
    long value = 0;
    ScriptEnvironment se((long*)&value);

    ASSERT_EQ(value, 0);
    se.registerFunction("hello-world", hello_world);
    ASSERT_EQ(value, 0);
    se.runScript(TESTS_DIR "/scripts/simple-value.scm");
    ASSERT_EQ(value, 1234);
}

TEST(ScriptInterpreter, TestReturnValueCall)
{
    scm_init_guile();

    long value = 0;
    ScriptEnvironment se((long*)&value);

    ASSERT_EQ(value, 0);
    se.registerFunction("get-value", get_value);
    se.registerFunction("set-value", set_value);
    ASSERT_EQ(value, 0);
    se.runScript(TESTS_DIR "/scripts/return-value.scm");
    ASSERT_EQ(value, 5678);
}

#include <gtest/gtest.h>

#include <array>
#include <common/logic/script_environment.hpp>
#include <memory>

#include "utils.hpp"

using namespace familyline::logic;

s7_pointer hello_world(s7_scheme* sc, s7_pointer args)
{
    s7_pointer value = s7_car(args);
    
    long* env = ScriptEnvironment::getGlobalEnv<long*>(sc);
    *env      = ScriptEnvironment::convertTypeFrom<long>(sc, value).value();
    return s7_t(sc);
}

s7_pointer get_value(s7_scheme* sc, s7_pointer args) { return s7_make_integer(sc, 5678); }

s7_pointer set_value(s7_scheme* sc, s7_pointer args)
{
    s7_pointer value = s7_car(args);
    
    long* env = ScriptEnvironment::getGlobalEnv<long*>(sc);
    *env      = ScriptEnvironment::convertTypeFrom<long>(sc, value).value();
    return s7_t(sc);
}

TEST(ScriptInterpreter, TestSimpleFunctionCall)
{
    long value = 0;
    ScriptEnvironment se((long*)&value);

    ASSERT_EQ(value, 0);
    se.registerFunction("hello-world", 1, hello_world);
    ASSERT_EQ(value, 0);
    se.runScript(TESTS_DIR "/scripts/simple-value.scm");
    ASSERT_EQ(value, 1234);
}

TEST(ScriptInterpreter, TestReturnValueCall)
{
    long value = 0;
    ScriptEnvironment se((long*)&value);

    ASSERT_EQ(value, 0);
    se.registerFunction("get-value", 0, get_value);
    se.registerFunction("set-value", 1, set_value);
    ASSERT_EQ(value, 0);
    se.runScript(TESTS_DIR "/scripts/return-value.scm");
    ASSERT_EQ(value, 5678);
}

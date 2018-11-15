/*  Unit test fixture for action manager
 *
 *  Copyright (C) 2017 Arthur Mendes.
 */

#include <gtest/gtest.h>

#include "../src/logic/ActionManager.hpp"
#include "TestObject.hpp"

#ifndef TEST_ACTIONMANAGER
#define TEST_ACTIONMANAGER

class TestActionManager : public familyline::logic::ActionManager
{
private:
    TestObject* tobj = new TestObject(2, 10, 1, 10);
    std::map<std::string, bool> ret;
    
public:
    virtual void RunAction(const char* name);
    
};

#endif //TEST_ACTIONMANAGER

class ActionManagerTest : public ::testing::Test {
protected:
    TestActionManager* am;
    
public:
    void SetUp();
    void TearDown();
};

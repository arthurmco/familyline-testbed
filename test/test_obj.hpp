/*  Unit test fixture for game objects
 *
 *  Copyright (C) 2017 Arthur Mendes.
 */

#include <gtest/gtest.h>

#include "logic/ObjectManager.hpp"
#include "logic/GameObject.hpp"
#include "TestObject.hpp"

class ObjectTest : public ::testing::Test {
protected:
    Tribalia::Logic::ObjectManager* _om;
    
public:
    void SetUp();
    void TearDown();
};

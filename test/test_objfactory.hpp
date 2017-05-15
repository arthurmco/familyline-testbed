/*  Unit test fixture for object factory
 *
 *  Copyright (C) 2017 Arthur M.
 */

#include <gtest/gtest.h>

#include "logic/ObjectManager.hpp"
#include "logic/GameObject.hpp"
#include "logic/ObjectFactory.hpp"
#include "TestObject.hpp"

class ObjectFactoryTest : public ::testing::Test {
protected:
    Tribalia::Logic::ObjectFactory* _of;

    TestObject* CreateObject();
    
public:
    void SetUp();
    void TearDown();
};

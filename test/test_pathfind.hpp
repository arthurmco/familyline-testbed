/*  Unit test fixture for pathfinding
 *
 *  Copyright (C) 2017 Arthur M.
 */

#include <gtest/gtest.h>

#include "logic/PathFinder.hpp"
#include "logic/ObjectManager.hpp"
#include "TestObject.hpp"

class PathfinderTest : public ::testing::Test {
protected:
    Tribalia::Logic::ObjectManager* _om;
    Tribalia::Logic::PathFinder* _pf;
    TestObject* o;
    
public:
    void SetUp();
    void TearDown();
};

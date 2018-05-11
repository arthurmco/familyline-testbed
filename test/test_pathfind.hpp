/*  Unit test fixture for pathfinding
 *
 *  Copyright (C) 2017 Arthur Mendes.
 */

#include <gtest/gtest.h>

#include "logic/PathFinder.hpp"
#include "logic/ObjectManager.hpp"
#include "TestObject.hpp"

class PathfinderTest : public ::testing::Test {
protected:
    Familyline::Logic::ObjectManager* _om;
    Familyline::Logic::PathFinder* _pf;
    TestObject* o;
    
public:
    void SetUp();
    void TearDown();
};

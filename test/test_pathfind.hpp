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
    familyline::logic::ObjectManager* _om;
    familyline::logic::PathFinder* _pf;
    std::weak_ptr<familyline::logic::GameObject> o;
    
public:
    void SetUp();
    void TearDown();
};

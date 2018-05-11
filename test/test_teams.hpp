/*  Unit test fixture for team testing
 *
 *  Copyright (C) 2017 Arthur Mendes.
 */

#include <gtest/gtest.h>

#include "logic/City.hpp"
#include "logic/Team.hpp"
#include "logic/TeamCoordinator.hpp"
#include "TestObject.hpp"

class TeamRelationTest : public ::testing::Test {
protected:
    Familyline::Logic::TeamCoordinator tc;
    Familyline::Logic::City *c11, *c12, *c21, *c22;
    
public:
    void SetUp();
    void TearDown();
};

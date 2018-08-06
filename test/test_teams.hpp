/*  Unit test fixture for team testing
 *
 *  Copyright (C) 2017 Arthur Mendes.
 */

#include <gtest/gtest.h>

#include "logic/CityManager.hpp"
#include "logic/City.hpp"
#include "logic/Team.hpp"
#include "TestObject.hpp"

class TeamRelationTest : public ::testing::Test {
protected:
    Familyline::Logic::CityManager* cm = nullptr;;
    
public:
    void SetUp();
    void TearDown();
};

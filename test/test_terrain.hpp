/*  Unit test fixture for the terrain opening and closing
 *
 *  Copyright (C) 2017 Arthur Mendes.
 */

#include <gtest/gtest.h>

#include "../src/logic/Terrain.hpp" 
#include "../src/logic/TerrainFile.hpp"

class TerrainTest : public ::testing::Test {
protected:
    Familyline::Logic::TerrainFile* tf;
    
public:
    void SetUp();
    void TearDown();
};

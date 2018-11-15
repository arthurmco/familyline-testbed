/*  Unit test fixture for the asset file loader
 *
 *  Copyright (C) 2017 Arthur Mendes.
 */

#include <gtest/gtest.h>

#include "../src/graphical/AssetFile.hpp"

class AssetFileTest : public ::testing::Test {
protected:
    familyline::graphics::AssetFile* af;
    
public:
    void SetUp();
    void TearDown();
};



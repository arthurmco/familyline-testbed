/*  Unit test fixture for the asset file loader
 *
 *  Copyright (C) 2017 Arthur M.
 */

#include <gtest/gtest.h>

#include "../src/graphical/AssetFile.hpp"

class AssetFileTest : public ::testing::Test {
protected:
    Tribalia::Graphics::AssetFile* af;
    
public:
    void SetUp();
    void TearDown();
};


